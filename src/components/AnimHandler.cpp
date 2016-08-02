#include <debugdraw/debugdraw.h>
#include "AnimHandler.h"
#include "utils/logger.h"
#include <algorithm>
#include <zenload/zCModelPrototype.h>
#include <engine/World.h>

using namespace Components;

AnimHandler::AnimHandler()
{
    m_ActiveAnimation.invalidate();
    m_AnimRootVelocity = Math::float3(0, 0, 0);
    m_LastProcessedFrame = static_cast<size_t>(-1);
    m_AnimationStateHash = 0;
    m_pWorld = nullptr;
}

/**
 * @brief Sets the mesh-lib this operates on. Does a copy inside, so the given object can be deleted.
 */
void AnimHandler::setMeshLib(const ZenLoad::zCModelMeshLib &meshLib)
{
    m_MeshLib = meshLib;

    m_NodeTransforms.resize(m_MeshLib.getNodes().size());
    m_ObjectSpaceNodeTransforms.resize(m_MeshLib.getNodes().size());

    for (auto &m : m_ObjectSpaceNodeTransforms)
        m = Math::Matrix::CreateIdentity();

    for (auto &m : m_NodeTransforms)
        m = Math::Matrix::CreateIdentity();

    setBindPose(true);
}

bool AnimHandler::addAnimation(const std::string &file)
{
    Handle::AnimationHandle h = m_pWorld->getAnimationAllocator().loadAnimationVDF(file);

    if(!h.isValid())
        return false;

    m_Animations.push_back(h);
    m_AnimationsByName[getAnimation(h).animation.getModelAniHeader().aniName] = h;

    return true;
}

/**
* @brief Sets the currently playing animation
*/
void AnimHandler::playAnimation(const std::string &animName)
{
    // Reset velocity
    m_AnimRootVelocity = Math::float3(0, 0, 0);

    if (animName.empty())
    {
        m_ActiveAnimation.invalidate();

        // Restore matrices from the bind-pose
        // because the animation won't modify all of the nodes
        for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
        {
            m_NodeTransforms[i] = Math::Matrix(m_MeshLib.getNodes()[i].transformLocal.mv);
        }

        return;
    }

    // find and apply given animation name
    auto it = m_AnimationsByName.find(animName);
    if (it == m_AnimationsByName.end())
    {
        //LogError() << "Failed to find animation: " << animName;
        m_ActiveAnimation.invalidate();
    }
    else
    {
        m_ActiveAnimation = (*it).second;
        m_AnimationFrame = 0.0f;

        // Restore matrices from the bind-pose
        // because the animation won't modify all of the nodes
        for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
        {
            m_NodeTransforms[i] = Math::Matrix(m_MeshLib.getNodes()[i].transformLocal.mv);
        }
    }
}

void AnimHandler::setAnimation(const std::string &animName)
{
    if (getActiveAnimationPtr() && getActiveAnimationPtr()->getModelAniHeader().aniName == animName)
        return;

    playAnimation(animName);
}

/**
* @brief Updates the currently playing animations
*/
void AnimHandler::updateAnimations(double deltaTime)
{
    if (m_LastProcessedFrame == m_AnimationFrame || !getActiveAnimationPtr())
        return; // Nothing to do here // TODO: There is, if interpolation was implemented!

    for (size_t i = 0; i < getActiveAnimationPtr()->getNodeIndexList().size(); i++)
    {
        // TODO: Lerp between this and the next frame
        size_t frameNum = static_cast<size_t>(m_AnimationFrame);
        size_t numAnimationNodes = getActiveAnimationPtr()->getNodeIndexList().size();
        uint32_t nodeIdx = getActiveAnimationPtr()->getNodeIndexList()[i];

        // Extract sample at the current frame/node
        auto &sample = getActiveAnimationPtr()->getAniSamples()[frameNum * numAnimationNodes + i];

        // Build transformation matrix from the sample-information
        // Note: Precomputing this is hard because of interpolation
        Math::Matrix trans = Math::Matrix::CreateFromQuaternion(Math::float4(sample.rotation.v));
        trans.Translation(Math::float3(sample.position.v));

        m_NodeTransforms[nodeIdx] = trans;
    }

    // Calculate actual node matrices
    for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
    {
        // TODO: There is a flag indicating whether the animation root should translate the vob position
        if (i == 0)
            m_NodeTransforms[i].Translation(Math::float3(0.0f, 0.0f, 0.0f));

        if (m_MeshLib.getNodes()[i].parentValid())
            m_ObjectSpaceNodeTransforms[i] =
                    m_ObjectSpaceNodeTransforms[m_MeshLib.getNodes()[i].parentIndex] * m_NodeTransforms[i];
        else
            m_ObjectSpaceNodeTransforms[i] = m_NodeTransforms[i];
    }

    // Increase current timeline-position
    float framesPerSecond = getActiveAnimationPtr()->getModelAniHeader().fpsRate;
    float numFrames = getActiveAnimationPtr()->getModelAniHeader().numFrames;
    size_t lastFrame = static_cast<size_t>(m_AnimationFrame);
    m_AnimationFrame += deltaTime * framesPerSecond;
    if (m_AnimationFrame >= numFrames)
        m_AnimationFrame = 0.0f;

    size_t frameNum = static_cast<size_t>(m_AnimationFrame);

    // Get velocity of the current animation
    // FIXME: Need better handling of animation end
    if (lastFrame != frameNum && frameNum != 0)
    {
        // Get sample of root node (Node 0) from the current and the last frame
        auto &sampleCurrent = getActiveAnimationPtr()->getAniSamples()[frameNum *
                getActiveAnimationPtr()->getNodeIndexList().size()];
        auto &sampleLast = getActiveAnimationPtr()->getAniSamples()[lastFrame *
                getActiveAnimationPtr()->getNodeIndexList().size()];

        // Scale velocity to seconds
        m_AnimRootVelocity = (Math::float3(sampleCurrent.position.v) - Math::float3(sampleLast.position.v)) *
                getActiveAnimationPtr()->getModelAniHeader().fpsRate;
        //LogInfo() << "Samples " << lastFrame << " -> " << frameNum  << " = " << m_AnimRootVelocity.toString();
    }

    // Updated the animation, update the hash-value
    m_AnimationStateHash++;
}

/**
* @brief Draws the skeleton of the animation
*/
void AnimHandler::debugDrawSkeleton(const Math::Matrix &transform)
{
    ddPush();
    for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
    {
        const ZenLoad::ModelNode &n = m_MeshLib.getNodes()[i];
        const auto &t2 = m_ObjectSpaceNodeTransforms[i].Translation();

        Math::float3 p2 = transform * t2;

        ddDrawAxis(p2.x, p2.y, p2.z, 0.04f);
        ddDrawAxis(p2.x, p2.y, p2.z, -0.04f);

        if (n.parentValid())
        {
            const auto &t1 = m_ObjectSpaceNodeTransforms[n.parentIndex].Translation();
            Math::float3 p1 = (transform * t1);

            ddSetColor(0xFFFFFFFF);
            ddMoveTo(p1.x, p1.y, p1.z);
            ddLineTo(p2.x, p2.y, p2.z);
        }
    }
    ddPop();
}

/**
* @brief Updates the given skeletalmesh-info
*/
void AnimHandler::updateSkeletalMeshInfo(Math::Matrix *target, size_t numMatrices)
{
    memcpy(target, m_ObjectSpaceNodeTransforms.data(),
           std::min(m_NodeTransforms.size(), numMatrices) * sizeof(Math::Matrix));

    //static float s_test = 0;
    //s_test += 0.1f;
    //for(int i = 0; i < ZenConvert::MAX_NUM_SKELETAL_NODES; i++)
    //{
    //	info.nodeTransforms[i] = Math::Matrix::CreateTranslation(Math::float3(0, sinf(s_test), 0));
    //}
}

ZenLoad::zCModelAni *AnimHandler::getActiveAnimationPtr()
{
    if (!m_ActiveAnimation.isValid())
        return nullptr;

    return &getAnimation(m_ActiveAnimation).animation;
}

bool AnimHandler::loadMeshLibFromVDF(const std::string &file, VDFS::FileIndex &idx)
{
    // Load heirachy
    ZenLoad::zCModelMeshLib lib(file + ".MDH", idx, 1.0f / 100.0f);
    setMeshLib(lib);

    // Load animations from MDS-file
    // TODO: This is different for G2!
    /*ZenLoad::zCModelPrototype modelPrototype(file + ".MDS", idx);

    for(auto& anim : modelPrototype.getAnimations())
    {
        LogInfo() << "Loading animation: " << file + "-" + anim.animationName + ".MAN";
        addAnimation(file + "-" + anim.animationName + ".MAN", idx, 1.0f / 100.0f);
    }*/

    addAnimation(file + "-" + "S_RUN" + ".MAN");

    return true;
}

void AnimHandler::setBindPose(bool force)
{
    if (force || getActiveAnimationPtr())
    {
        m_ActiveAnimation.invalidate();

        // Bind-pose
        for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
            m_NodeTransforms[i] = Math::Matrix(m_MeshLib.getNodes()[i].transformLocal.mv);

        // Calculate actual node matrices
        for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
        {
            // TODO: There is a flag indicating whether the animation root should translate the vob position
            if (i == 0)
                m_NodeTransforms[i].Translation(Math::float3(0.0f, 0.0f, 0.0f));

            if (m_MeshLib.getNodes()[i].parentValid())
                m_ObjectSpaceNodeTransforms[i] =
                        m_ObjectSpaceNodeTransforms[m_MeshLib.getNodes()[i].parentIndex] * m_NodeTransforms[i];
            else
                m_ObjectSpaceNodeTransforms[i] = m_NodeTransforms[i];
        }
    }
}

Animations::Animation &AnimHandler::getAnimation(Handle::AnimationHandle h)
{
    return m_pWorld->getAnimationAllocator().getAnimation(h);
}
