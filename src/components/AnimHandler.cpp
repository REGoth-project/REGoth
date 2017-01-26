#include <algorithm>

#include <debugdraw/debugdraw.h>
#include <engine/World.h>
#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCModelPrototype.h>

#include "AnimHandler.h"

using namespace Components;

AnimHandler::AnimHandler()
{
    m_ActiveAnimation.invalidate();
    m_AnimRootVelocity = Math::float3(0, 0, 0);
    m_AnimRootPosition = Math::float3(0, 0, 0);
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

    m_SpeedMultiplier = 1.0f;
}

bool AnimHandler::addAnimation(const std::string &name)
{
    // Add overlay/lib
    std::string file = m_ActiveOverlay + "-" + name + ".MAN";

    Handle::AnimationHandle h = m_pWorld->getAnimationAllocator().loadAnimationVDF(file);

    // Try again with lib only
	if(!h.isValid())
	{
		if(m_ActiveOverlay != m_MeshLibName)
		{
			file = m_MeshLibName + "-" + name + ".MAN";
			h = m_pWorld->getAnimationAllocator().loadAnimationVDF(file);

			if(!h.isValid())
				return false;
		}
		else
		{
			return false;
		}
	}

    m_Animations.push_back(h);
    m_AnimationsByName[getAnimation(h).animation.getModelAniHeader().aniName] = h;

    return true;
}

/**
* @brief Sets the currently playing animation
*/
void AnimHandler::playAnimation(const std::string &animName)
{
    //if(m_MeshLibName == "CHESTSMALL_OCCRATESMALL")
    //    LogInfo() << "Playing animation '" << animName << "' on Model: " << m_MeshLibName;

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

    // If we currently don't have this animation, try to load it
    if(!hasAnimation(animName))
        addAnimation(animName);

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
        m_LoopActiveAnimation = false;
        m_LastProcessedFrame = (size_t)-1;

        // Restore matrices from the bind-pose
        // because the animation won't modify all of the nodes
        for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
        {
            m_NodeTransforms[i] = Math::Matrix(m_MeshLib.getNodes()[i].transformLocal.mv);
        }

        // Run first frame of the animation to get around the return to the bind-pose above causing the model to jump
        updateAnimations(0.0);
    }
}

void AnimHandler::setAnimation(const std::string &animName)
{
    if (getActiveAnimationPtr() && getActiveAnimationPtr()->getModelAniHeader().aniName == animName)
        return;

    playAnimation(animName);

    m_LoopActiveAnimation = true;
}

/**
* @brief Updates the currently playing animations
*/
void AnimHandler::updateAnimations(double deltaTime)
{
    if(!getActiveAnimationPtr())
        return;

    // Increase current timeline-position
    float framesPerSecond = getActiveAnimationPtr()->getModelAniHeader().fpsRate * m_SpeedMultiplier;
    float numFrames = getActiveAnimationPtr()->getModelAniHeader().numFrames;
    size_t lastFrame = static_cast<size_t>(m_AnimationFrame);
    m_AnimationFrame += deltaTime * framesPerSecond;
    if (m_AnimationFrame >= numFrames)
    {
        // If there is a next animation, play this now
        std::string next = getActiveAnimationPtr()->getModelAniHeader().nextAniName;
        if(!next.empty())
        {
            //if(next != "S_RUN" && next != "S_RUNL")
            //    LogInfo() << "Setting next Ani: " << next;

            bool oldLoop = m_LoopActiveAnimation;
            playAnimation(next);

            // Make sure we loop that one if we wanted to loop the starting animation
            m_LoopActiveAnimation = oldLoop;

            return;
        }

        if(m_LoopActiveAnimation)
            m_AnimationFrame = 0.0f;
        else
        {
            stopAnimation(); // Animation done and not looping
            return;
        }
    }

    size_t frameNum = static_cast<size_t>(m_AnimationFrame);

    // Check if this changed something on our animation
    //if (m_LastProcessedFrame == static_cast<size_t>(m_AnimationFrame))
    //    return; // Nothing to do here // TODO: Do this distance-based!

    m_LastProcessedFrame = static_cast<size_t>(m_AnimationFrame);

    // frameNum contains the current frame, find out what the next frame will be and how far we're in there already
    size_t frameNext = (frameNum + 1) % Math::trunc(numFrames); // FIXME: What happens on non-looped animation on the last frame?
    float frameFract = fmod(m_AnimationFrame, 1.0f); // Get fraction of this frame we are currently at


    for (size_t i = 0; i < getActiveAnimationPtr()->getNodeIndexList().size(); i++)
    {
        size_t frameNum = static_cast<size_t>(m_AnimationFrame);
        size_t numAnimationNodes = getActiveAnimationPtr()->getNodeIndexList().size();
        uint32_t nodeIdx = getActiveAnimationPtr()->getNodeIndexList()[i];

        // Extract sample at the current frame/node
        auto &sample = getActiveAnimationPtr()->getAniSamples()[frameNum * numAnimationNodes + i];
        auto &sampleNext = getActiveAnimationPtr()->getAniSamples()[frameNext * numAnimationNodes + i];

        // Interpolate between frames
        Math::float4 interpRotation = Math::float4::slerp(Math::float4(sample.rotation.v),
                                                          Math::float4(sampleNext.rotation.v),
                                                          frameFract);

        Math::float3 interpPosition = Math::float3::lerp(Math::float3(sample.position.v),
                                                         Math::float3(sampleNext.position.v),
                                                         frameFract);

        // Build transformation matrix from the sample-information
        // Note: Precomputing this is hard because of interpolation
        Math::Matrix trans = Math::Matrix::CreateFromQuaternion(interpRotation);
        trans.Translation(interpPosition);

        m_NodeTransforms[nodeIdx] = trans;
    }

    // Calculate actual node matrices
    for (size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
    {
        // TODO: There is a flag indicating whether the animation root should translate the vob position
        if (i == 0)
        {
            m_AnimRootPosition = m_NodeTransforms[i].Translation();
            m_NodeTransforms[i].Translation(Math::float3(0.0f, 0.0f, 0.0f));
        }

        if (m_MeshLib.getNodes()[i].parentValid())
            m_ObjectSpaceNodeTransforms[i] =
                    m_ObjectSpaceNodeTransforms[m_MeshLib.getNodes()[i].parentIndex] * m_NodeTransforms[i];
        else
            m_ObjectSpaceNodeTransforms[i] = m_NodeTransforms[i];
    }

    // Updated the animation, update the hash-value
    m_AnimationStateHash++;

    // Get velocity of the current animation
    // FIXME: Need better handling of animation end
    if (!getActiveAnimationPtr()->getAniSamples().empty() && lastFrame != frameNum && frameNum != 0)
    {
        // Get sample of root node (Node 0) from the current and the last frame
        auto &sampleCurrent = getActiveAnimationPtr()->getAniSamples()[frameNum *
                getActiveAnimationPtr()->getNodeIndexList().size()];
        auto &sampleLast = getActiveAnimationPtr()->getAniSamples()[lastFrame *
                getActiveAnimationPtr()->getNodeIndexList().size()];

        // Scale velocity to seconds
        m_AnimRootVelocity = (Math::float3(sampleCurrent.position.v) - Math::float3(sampleLast.position.v));
        //LogInfo() << "Samples " << lastFrame << " -> " << frameNum  << " = " << m_AnimRootVelocity.toString();
        m_AnimRootNodeVelocityUpdatedHash = m_AnimationStateHash;

    }


}

Math::float3 AnimHandler::getRootNodePositionAt(size_t frame)
{
    if(!getActiveAnimationPtr())
        return Math::float3(0,0,0);

    size_t numAnimationNodes = getActiveAnimationPtr()->getNodeIndexList().size();

    if(frame == (size_t)-1)
        frame = getActiveAnimationPtr()->getModelAniHeader().numFrames - 1;

    // Root node is always node 0
    return Math::float3(getActiveAnimationPtr()->getAniSamples()[numAnimationNodes * frame].position.v);
}


Math::float3 AnimHandler::getRootNodeVelocityAvg()
{
    if(!getActiveAnimationPtr())
        return Math::float3(0,0,0);

    // Get length of the animation in seconds
    float length = getActiveAnimationPtr()->getModelAniHeader().numFrames
                   / (getActiveAnimationPtr()->getModelAniHeader().fpsRate * m_SpeedMultiplier);

    size_t numFrames = getActiveAnimationPtr()->getModelAniHeader().numFrames;
    Math::float3 start = getRootNodePositionAt(0);
    Math::float3 end = getRootNodePositionAt(numFrames - 1);

    return (end - start) / length ;
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

        ddSetTransform((transform * m_ObjectSpaceNodeTransforms[i]).v);
        ddDrawAxis(0,0,0, 0.04f);
        ddDrawAxis(0,0,0, -0.04f);

        if (n.parentValid())
        {
            const auto &t1 = m_ObjectSpaceNodeTransforms[n.parentIndex].Translation();
            Math::float3 p1 = (transform * t1);

            ddSetTransform(nullptr);
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

    if(m_MeshLibName == "CHESTSMALL_OCCRATESMALL")
        LogInfo() << "Updating: " << m_MeshLibName;

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
    ZenLoad::zCModelMeshLib lib;

    // Load heirachy
    if(idx.hasFile(file + ".MDH"))
        lib = ZenLoad::zCModelMeshLib(file + ".MDH", idx, 1.0f / 100.0f);
    else if(idx.hasFile(file + ".MDL")) // Some mobs have .MDL
        lib = ZenLoad::zCModelMeshLib(file + ".MDL", idx, 1.0f / 100.0f);
    else if(idx.hasFile(file + ".MDM")) // Some mobs have .MDL
        lib = ZenLoad::zCModelMeshLib(file + ".MDM", idx, 1.0f / 100.0f);

    if(!lib.isValid())
        LogWarn() << "Could not load MeshLib for Visual: " << file;

    setMeshLib(lib);

    // Save name of this meshlib loading more animations later
    m_MeshLibName = file;
    m_ActiveOverlay = m_MeshLibName;

    // Load animations from MDS-file
    // TODO: This is different for G2!
    /*ZenLoad::zCModelPrototype modelPrototype(file + ".MDS", idx);

    for(auto& anim : modelPrototype.getAnimations())
    {
        LogInfo() << "Loading animation: " << file + "-" + anim.animationName + ".MAN";
        addAnimation(file + "-" + anim.animationName + ".MAN", idx, 1.0f / 100.0f);
    }*/

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

void AnimHandler::setOverlay(const std::string& mds)
{
    if(mds.find_last_of('.') != std::string::npos)
    {
        m_ActiveOverlay = mds.substr(0, mds.find_last_of('.'));
    }else
    {
        m_ActiveOverlay = mds;
    }

    if(mds.empty())
        m_ActiveOverlay = m_MeshLibName;

    // Try to switch loaded animations
    for(auto& a : m_AnimationsByName)
    {
        std::string name = getAnimation(a.second).animation.getModelAniHeader().aniName;
        Handle::AnimationHandle h = m_pWorld->getAnimationAllocator().loadAnimationVDF(m_ActiveOverlay + "-" + name + ".MAN");

        // Update with overlay variant
        if(h.isValid())
            a.second = h;
    }
}

Math::float3 AnimHandler::getRootNodePosition()
{
    return m_AnimRootPosition;
}
