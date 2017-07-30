//
// Created by desktop on 24.01.17.
//

#include "PfxVisual.h"
#include <stdlib.h>
#include <ZenLib/utils/logger.h>
#include <bx/fpumath.h>
#include <components/EntityActions.h>
#include <debugdraw/debugdraw.h>
#include <engine/AsyncAction.h>
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <logic/PfxManager.h>

Logic::PfxVisual::PfxVisual(World::WorldInstance& world, Handle::EntityHandle entity)
    : VisualController(world, entity)
    , m_TimeSinceLastSpawn(0.0f)
    , m_ppsScaleKey(0.0f)
    , m_spawnPosition(0.0f)
{
    Components::Actions::initComponent<Components::PfxComponent>(m_World.getComponentAllocator(), entity);
}

Logic::PfxVisual::~PfxVisual()
{
}

bool Logic::PfxVisual::load(const std::string& visual)
{
    VisualController::load(visual);

    // Strip .PFX ending
    std::string sym = visual.substr(0, visual.find_last_of('.'));

    // https://wiki.worldofgothic.de/doku.php?id=partikel_effekte
    // Get data of the PFX over to a new format

    if (!m_World.getPfxManager().hasPFX(sym))
    {
        LogWarn() << "Failed to find PFX: " << sym;
        return false;
    }

    m_Emitter = m_World.getPfxManager().getParticleFX(sym);

    // Need that one. Or should give a default value of 1?
    assert(!m_Emitter.ppsScaleKeys.empty());

    // Init particle-systems dynamic vertex-buffer
    // Needs to happen on the mainthread
    auto job = [this](Engine::BaseEngine* engine) {
        getPfxComponent().m_ParticleVB = bgfx::createDynamicVertexBuffer(6,
                                                                         Meshes::WorldStaticMeshVertex::ms_decl,  // FIXME: May want to use a smaller one
                                                                         BGFX_BUFFER_ALLOW_RESIZE);
    };
    Engine::AsyncAction::executeInThread(job, m_World.getEngine(), Engine::ExecutionPolicy::MainThread).wait();

    getPfxComponent().m_Texture = m_World.getTextureAllocator().loadTextureVDF(m_Emitter.visName);

    /*
    	SrcBlend = BF_SRC_ALPHA;
		DestBlend = BF_ONE;
		BlendOp = BO_BLEND_OP_ADD;
		SrcBlendAlpha = BF_ONE;
		DestBlendAlpha = BF_ZERO;
		BlendOpAlpha =	BO_BLEND_OP_ADD;
     */
    //uint64_t state_add = BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_ONE);
    //state_add |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);

    uint64_t state_add = BGFX_STATE_BLEND_ADD;
    uint64_t state_default = BGFX_STATE_DEFAULT & ~BGFX_STATE_DEPTH_WRITE & ~BGFX_STATE_ALPHA_WRITE;

    switch (m_Emitter.visAlphaFunc)
    {
        case PfxManager::EBM_None:
            getPfxComponent().m_bgfxRenderState = state_default;
            break;
        case PfxManager::EBM_Blend:
            getPfxComponent().m_bgfxRenderState = (state_default & ~BGFX_STATE_DEPTH_WRITE) | BGFX_STATE_BLEND_ALPHA;
            break;
        case PfxManager::EBM_Add:
            getPfxComponent().m_bgfxRenderState = (state_default & ~BGFX_STATE_DEPTH_WRITE) | state_add;
            break;
        case PfxManager::EBM_Mul:
            getPfxComponent().m_bgfxRenderState = (state_default & ~BGFX_STATE_DEPTH_WRITE) | BGFX_STATE_BLEND_MULTIPLY;
            break;
    }

    return true;
}

Components::PfxComponent& Logic::PfxVisual::getPfxComponent()
{
    return m_World.getEntity<Components::PfxComponent>(m_Entity);
}

void Logic::PfxVisual::onUpdate(float deltaTime)
{
    Components::PfxComponent& pfx = getPfxComponent();

    Controller::onUpdate(deltaTime);

    // Spawn new particles. Need to accumulate deltaTime so the floor doesn't keep us from spawning any particles
    // on high fps-rates
    m_TimeSinceLastSpawn += deltaTime;

    // Update rates
    m_ppsScaleKey += deltaTime * m_Emitter.ppsFPS;
    m_shpScaleKey += deltaTime * m_Emitter.shpScaleFPS;

    // Loop ppsScaleKeys if wanted
    if (Math::ifloor(m_ppsScaleKey) >= static_cast<int>(m_Emitter.ppsScaleKeys.size()) && !m_Emitter.ppsIsLooping)
        m_ppsScaleKey = static_cast<float>(m_Emitter.ppsScaleKeys.size()) + 0.5f;  // Keep on high value
    else
        m_ppsScaleKey = 0.0f;

    if (Math::ifloor(m_shpScaleKey) >= static_cast<int>(m_Emitter.shpScaleKeys.size()) && !m_Emitter.shpScaleIsLooping)
        m_shpScaleKey = static_cast<float>(m_Emitter.shpScaleKeys.size()) + 0.5f;  // Keep on high value
    else
        m_shpScaleKey = 0.0f;

    // Perform spawning rate modulation
    float ppsKeyFrac = fmod(m_ppsScaleKey, 1.0f);  // For interpolation
    float ppsMod1 = m_Emitter.ppsScaleKeys[Math::ifloor(m_ppsScaleKey)];
    float ppsMod2 = m_Emitter.ppsScaleKeys[(Math::ifloor(m_ppsScaleKey) + 1) % m_Emitter.ppsScaleKeys.size()];
    float ppsModTotal = m_Emitter.ppsIsSmooth ? bx::flerp(ppsMod1, ppsMod2, ppsKeyFrac) : ppsMod1;

    int toSpawn = Math::ifloor(m_Emitter.ppsValue * m_TimeSinceLastSpawn * ppsModTotal);
    if (toSpawn > 1)
    {
        for (int i = 0; i < toSpawn; i++)
            spawnParticle();

        m_TimeSinceLastSpawn = 0.0f;
    }

    // Update particle values
    for (Components::PfxComponent::Particle& p : pfx.m_Particles)
        updateParticle(p, deltaTime);

    for (int i = 0; i < static_cast<int>(pfx.m_Particles.size()); i++)
    {
        Components::PfxComponent::Particle& p = pfx.m_Particles[i];

        if (p.lifetime < 0)
        {
            // Kill particle. Move the last one into the free slot and reduce the vector size
            // to keep the memory continuous
            pfx.m_Particles[i] = pfx.m_Particles.back();
            pfx.m_Particles.pop_back();

            // Do one step back, since we have a new particle in this slot now
            i--;
        }
    }
}

void Logic::PfxVisual::spawnParticle()
{
    Components::PfxComponent& pfx = getPfxComponent();
    pfx.m_Particles.emplace_back();

    Components::PfxComponent::Particle& p = pfx.m_Particles.back();

    // Perform shape scale modulation
    float shpKeyFrac = fmod(m_ppsScaleKey, 1.0f);  // For interpolation
    float shpMod1 = m_Emitter.ppsScaleKeys[Math::ifloor(m_ppsScaleKey)];
    float shpMod2 = m_Emitter.ppsScaleKeys[(Math::ifloor(m_ppsScaleKey) + 1) % m_Emitter.ppsScaleKeys.size()];
    float shpModTotal = m_Emitter.ppsIsSmooth ? bx::flerp(shpMod1, shpMod2, shpKeyFrac) : shpMod1;

    Math::float3 dir(0, 0, 0);
    switch (m_Emitter.dirMode)
    {
        case PfxManager::EDM_NONE:
            dir = Math::float3(Utils::frandF2(),
                               Utils::frandF2(),
                               Utils::frandF2())
                      .normalize();
            break;

        case PfxManager::EDM_DIR:
            dir = m_Emitter.dirAngleBox.min + Math::float3(Utils::frandF2() * m_Emitter.dirAngleBoxDim.x,
                                                           Utils::frandF2() * m_Emitter.dirAngleBoxDim.y,
                                                           Utils::frandF2() * m_Emitter.dirAngleBoxDim.z);
            dir = dir.normalize();
            break;

        case PfxManager::EDM_TARGET:
            break;  // TODO
        case PfxManager::EDM_MESH:
            break;  // TODO
    }

    p.velocity = dir * (m_Emitter.velAvg + Utils::frandF2() * m_Emitter.velVar);
    p.position = m_Emitter.shpOffset;

    Math::float3 offset(0, 0, 0);
    switch (m_Emitter.shpType)
    {
        case PfxManager::ES_POINT:
            break;  // Nothing, just offset
        case PfxManager::ES_LINE:
            break;
        case PfxManager::ES_BOX:
        {
            // This will result in double the size of the actual box, but this is how it was implemented by PB.
            offset = Math::float3(Utils::frandF2() * m_Emitter.shpDim.x,
                                  Utils::frandF2() * m_Emitter.shpDim.y,
                                  Utils::frandF2() * m_Emitter.shpDim.z);
        }
        break;
        case PfxManager::ES_CIRCLE:
        {
            // TODO: Walk-placement
            float r = Utils::frand() * Math::PI * 2.0f;

            if (!m_Emitter.shpIsVolume)
            {
                offset = Math::float3(cos(r) * m_Emitter.shpDim.x, 0, sin(r) * m_Emitter.shpDim.x);
            }
            else
            {
                float v = Utils::frand();
                offset = Math::float3(cos(r) * m_Emitter.shpDim.x * v, 0, sin(r) * m_Emitter.shpDim.x * v);
            }
        }
        break;
        case PfxManager::ES_SPHERE:
        {
            float rx = Utils::frandF2();
            float ry = Utils::frandF2();
            float rz = Utils::frandF2();

            if (!m_Emitter.shpIsVolume)
            {
                offset = Math::float3(rx, ry, rz).normalize() * m_Emitter.shpDim.x;
            }
            else
            {
                float v = Utils::frand();
                offset = Math::float3(rx, ry, rz).normalize() * m_Emitter.shpDim.x * v;
            }
        }
        break;
        case PfxManager::ES_MESH:
            break;
    }

    p.position += getEntityTransform().Translation() + offset * shpModTotal;

    // Lifetime with variance
    p.lifetime = m_Emitter.lspPartAvg + Utils::frandF2() * m_Emitter.lspPartVar;

    // Compute particle state velocities
    // They compute the alpha/size velocities by adding +5 ms to the lifetime...
    float lifetimeInv = 1.0f / (p.lifetime + (5.0f / 1000.0f));

    p.alpha = m_Emitter.visAlphaStart;
    p.alphaVel = (m_Emitter.visAlphaEnd - m_Emitter.visAlphaStart) * lifetimeInv;

    p.size = m_Emitter.visSizeStart;
    if (m_Emitter.visSizeEndScale != 1)
        p.sizeVel = (m_Emitter.visSizeStart * m_Emitter.visSizeEndScale - m_Emitter.visSizeStart) * lifetimeInv;
    else
        p.sizeVel = Math::float2(0, 0);

    p.color = m_Emitter.visTexColorStart;
    if (m_Emitter.visTexColorStart != m_Emitter.visTexColorEnd)
        p.colorVel = (m_Emitter.visTexColorEnd - m_Emitter.visTexColorStart) * lifetimeInv;
    else
        p.colorVel = Math::float3(0, 0, 0);
}

void Logic::PfxVisual::updateParticle(Components::PfxComponent::Particle& p, float deltaTime)
{
    Components::PfxComponent& pfx = getPfxComponent();

    // Check for out-of-time first
    p.lifetime -= deltaTime;

    // Update transforms
    p.velocity += m_Emitter.flyGravity * deltaTime;
    p.position += p.velocity * deltaTime;

    p.color += p.colorVel * deltaTime;
    p.size += p.sizeVel * deltaTime;
    p.alpha += p.alphaVel * deltaTime;

    // Compute actual color for this frame
    float alpha = std::max(0.0f, std::min(1.0f, p.alpha)) * 0.5f;  // FIXME: Hack! * 0.5f is just here because particles would be too bright for some strange reason...
    Math::float3 color = Math::float3(std::max(0.0f, std::min(1.0f, p.color.x)),
                                      std::max(0.0f, std::min(1.0f, p.color.y)),
                                      std::max(0.0f, std::min(1.0f, p.color.z)));

    // Compute new alpha-value
    Math::float4 particleColor = m_Emitter.visAlphaFunc != PfxManager::EBM_Add
                                     ? Math::float4(color.x,
                                                    color.y,
                                                    color.z,
                                                    alpha)
                                     : Math::float4(color.x * alpha,
                                                    color.y * alpha,
                                                    color.z * alpha,
                                                    alpha);  // Need to modulate color on ADD-mode

    p.particleColorU8 = particleColor.toRGBA8();
}
