#pragma once
#include <string>
#include <ZenLib/daedalus/DaedalusGameState.h>
#include <math/mathlib.h>
#include <utils/Utils.h>

namespace Daedalus
{
    class DaedalusVM;
    namespace GEngineClasses
    {
        struct C_ParticleFX;
    }
}

namespace Engine
{
    class BaseEngine;
}

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    /**
     * Class which holds on to a Daedalus-VM loaded with the PARTICLEFX.DAT
     */
    class PfxManager
    {
    public:
        enum EmitterShape
        {
            ES_POINT,
            ES_LINE,
            ES_BOX,
            ES_CIRCLE,
            ES_SPHERE,
            ES_MESH
        };

        enum EmitterFOR
        {
            EFOR_WORLD,
            EFOR_OBJECT
        };

        enum EmitterDirMode
        {
            EDM_NONE,
            EDM_DIR,
            EDM_TARGET,
            EDM_MESH
        };

        enum EmitterVisOrient
        {
            EVO_NONE,
            EVO_VELO_ALIGNED,
            EVO_VOB_XZPLANE,
            EVO_VELO_ALIGNED3D,
        };

        enum EmitterDistribType
        {
            EDT_RAND,
            EDT_UNIFORM,  // Does not work in the original
            EDT_WALK,
        };

        /**
         * https://wiki.worldofgothic.de/doku.php?id=partikel_effekte
         * Die Kollisionserkennung kennt verschiedene Modi:
         * 0 = Keine Kollisionserkennung;
         * 1 = Die Partikel werden abgebremst und reflektiert;
         * 2 = die Partikel werden beschleunigt und reflektiert;
         * 3 = die Partikel halten an;
         * 4 = die Partikel werden entfernt
         */
        enum EmitterCollDetType
        {
            ECDT_None = 0,
            ECDT_Slow_Reflect = 1,
            ECDT_Accel_Reflect = 2,
            ECDT_Stop = 3,
            ECDT_Kill = 4
        };

        enum EmitterBlendMode
        {
            EBM_None,
            EBM_Blend,
            EBM_Add,
            EBM_Mul
        };

        struct Emitter
        {
            // https://wiki.worldofgothic.de/doku.php?id=partikel_effekte
            float ppsValue;                   // Particles per second
            std::vector<float> ppsScaleKeys;  // Factors for ppsValue by time
            bool ppsIsLooping;                // Whether to loop the factors in ppsScaleKeys
            bool ppsIsSmooth;                 // Whether to lerp between ppsScaleKeys or just jump straight to them
            float ppsFPS;                     // How fast to go through the list in ppsScaleKeys
            std::string ppsCreateEm;          // Creates an other Emitter on top of this one (apparently barely used)
            float ppsCreateEmDelay;           // Delay before the emitter in "ppsCreateEm" will be created

            EmitterShape shpType;               // Type of shape the emitter should have
            EmitterFOR shpFOR;                  // "Frame-Of-Reference" for the emitter. Setting this to "world" will not rotate the emitter with it's object.
            Math::float3 shpOffset;             // Offset from the root of the parent-object in meters
            EmitterDistribType shpDistribType;  // (Circle, line) How to distribute the spawning points on the spawn-shape
            float shpDistribWalkSpeed;          // If shpDistribType == WALK, how fast to walk along the spawn-shape
            bool shpIsVolume;                   // Whether the spawning-shape should be seen as volume (will not only spawn particles on the edges)
            Math::float3 shpDim;                // Size of the shape. Radius for Circle and Sphere, length for line. 3 lengths for box.
            std::string shpMesh;                // if shpType == MESH, this is the mesh to be rendered
            bool shpMeshRender;                 // Whether to draw the mesh set in shpMesh
            std::vector<float> shpScaleKeys;    // Scale for the spawning shape over time
            bool shpScaleIsLooping;             // Whether to loop factors in shpScaleKeys;
            bool shpScaleIsSmooth;              // Whether to lerp between shpScaleKeys or just jump straight to them
            float shpScaleFPS;                  // How fast to go through the list in ppsScaleKeys

            EmitterDirMode dirMode;         // Direction of the particles
            EmitterFOR dirFOR;              // "Frame-Of-Reference" for the particles. See shpFOR.
            EmitterFOR dirModeTargetFOR;    // Same as dirFOR but for target?
            Math::float3 dirModeTargetPos;  // Position the particles go to if dirMode == TARGET.
            float dirAngleHead;             // Angle (Degrees) the particles fly to if dirMode == DIR (left/right)
            float dirAngleHeadVar;          // Variance in dirAngleHead
            float dirAngleElev;             // Angle (Degrees) the particles fly to if dirMode == DIR (up/down)
            float dirAngleElevVar;          // Variance in dirAngleElev
            Math::float3 dirDirection;      // (REGOTH) Calculated direction-vector
            Utils::BBox3D dirAngleBox;
            Math::float3 dirAngleBoxDim;

            float velAvg;  // Initial speed of the particles
            float velVar;  // Variance of velAvg

            float lspPartAvg;  // Lifespan of the particles
            float lspPartVar;  // Variance of lspPartAvg

            Math::float3 flyGravity;  // Acceleration (gravity) for each particle (No FOR here)
            EmitterCollDetType flyCollDet;

            std::string visName;              // Name of the texture for each particle
            EmitterVisOrient visOrientation;  // Orientation for each particle
            bool visTexIsQuadPoly;            // Whether to draw a complete polygon (Always the case in REGoth)
            float visTexAniFPS;               // Number of texture-animation frames per second
            bool visTexAniIsLooping;          // Whether to loop the texture animation
            Math::float3 visTexColorStart;    // Start of texture color-modulation interpolation
            Math::float3 visTexColorEnd;      // End of texture color-modulation interpolation
            Math::float2 visSizeStart;        // Size of the particle on start. (visOrientation == None: Screen axis. X = Target direction otherwise)
            float visSizeEndScale;            // Scale of visSizeStart at the end of the lifetime of a particle
            EmitterBlendMode visAlphaFunc;    // Blend-mode for the particles
            float visAlphaStart;              // Multiplied with the particles alpha-value. Linear interpolation over lifetime to visAlphaEnd
            float visAlphaEnd;                // See visAlphaStart

            float trlFadeSpeed;      // Fade speed for particle-trails
            std::string trlTexture;  // Texture to use as trail
            float trlWidth;          // Width of the trail

            float mrkFadeSpeed;      // How quick a mark on the worldmesh should decay
            std::string mrkTexture;  // Texture to leave behind on the worldmesh on impact
            float mrkSize;           // Scale if the mark

            bool useEmittersFOR;                   // If true, particles will move with the emitter
            std::pair<float, float> timeStartEnd;  // Time when to render this emitter. Can be something like (22, 3) to render the effect only during nighttime
            bool isAmbientPFX;                     // Not rendered if the player choose to not render AmbientPFX in the games settings
        };

        PfxManager(World::WorldInstance& world);
        ~PfxManager();

        /**
         * Checks whether the given pfx-instance exists
         * @param name instance to check
         * @return Whether the given instance exists inside PARTICLEFX.DAT
         */
        bool hasPFX(const std::string& name);

        /**
         * Looks up a particle-system instance and returns its settings
         * @param name Name of the symbol the instance has inside PARTICLEFX.DAT
         * @return Settings for the given FX.
         */
        const Emitter& getParticleFX(const std::string& name);

    private:
        /**
         * Loads the PARTICLEFX.DAT
         * @return Success
         */
        bool loadParticleFXDAT();

        /**
         * Daedalus-VM loaded with the PARTICLEFX.DAT
         */
        Daedalus::DaedalusVM* m_pVM;

        /**
         * World this is running in
         */
        World::WorldInstance& m_World;

        /**
         * Pfx already gotten from script
         */
        std::map<size_t, Emitter> m_PfxCache;

        /**
         * Default emitter to return if an invalid one was requested
         */
        Emitter m_DefaultEmitter;
    };
}
