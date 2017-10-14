#pragma once

#include "Texture.h"
#include <handle/HandleDef.h>
#include <math/mathlib.h>
#include <array>

namespace World
{
    class WorldInstance;
}

namespace Content
{
    /**
     * Skystates, in order
     */
    enum ESkyPresetType : int
    {
        ESPT_Day1 = 0,
        ESPT_Day2,
        ESPT_Evening,
        ESPT_Night0,
        ESPT_Night1,
        ESPT_Night2,
        ESPT_Dawn,
        ESPT_Day0,
        ESPT_NUM_PRESETS
    };

    class SkyConfig;
    class Sky
    {
    public:
        /**
         * There is one sky and one clouds-layer
         */
        enum
        {
            NUM_SKY_LAYERS = 2
        };
        struct SkyLayerData
        {
            // This layers texture
            Handle::TextureHandle texture;
            std::string textureNameBase;

            // Other
            float textureAlpha;
            float textureScale;
            Math::float2 textureSpeed;
        };

        /**
         * One skystate which can be interpolated into
         */
        struct SkyState
        {
            // Time when this state becomes active
            // time in days since last 12:00
            float time;

            // Color values
            Math::float3 baseColor;
            Math::float3 fogColor;
            Math::float3 domeColorUpper;

            // Base-fog distance modifier
            float fogDistance;

            // Whether the sun should be active. If false, the moon is.
            bool sunEnabled;

            // Information about the cloud and sky layers
            std::array<SkyLayerData, NUM_SKY_LAYERS> layers;
        };

        Sky(World::WorldInstance& world);
        ~Sky();

        /**
         * Updates the sky and the colors
         */
        void interpolate();

        /**
         * Initializes the given skystate to the given type
         * @param type Type of state we want to make
         * @param s state to fill with values
         * @param texAlloc Texture allocator to take the data from
         */
        static void initSkyState(World::WorldInstance& world, ESkyPresetType type, SkyState& s, Textures::TextureAllocator& texAlloc);

        /**
         * Fills the m_SkyStates with their preset values
         */
        void fillSkyStates();

        /**
         * To be called when the name of the current level changed, so the sky can adapt to custom-textures the world may has.
         * @param newWorldName
         */
        void onWorldNameChanged(const std::string& newWorldName);

        /**
         * Returns the current LUT-array. Each entry of this array maps an 8-bit luminance to another color.
         * @return Pointer to the LUT
         */
        const Math::float4* getLUTPtr() const { return m_LUT; }

        /**
         * Outputs the two colors to lerp intensity values with to turn it into a sky-colored pixel
         */
        void getSkyColors(Math::float4& color0, Math::float4& color1);

        /**
         * @return current interpolated sky-state
         */
        const SkyState& getMasterState() const { return m_MasterState; }
        /**
         * Calculates the near- and farplanes for the fog
         * @param cameraWorld current cameras world position
         * @param near target for nearplane
         * @param far target for farplane
         */
        void getFogValues(const Math::float3& cameraWorld, float& nearFog, float& farFog, Math::float3& fogColor) const;

        // Testing only
        const SkyState& getMasterState() { return m_MasterState; }

        /**
         * Sets the farplane, needed to do the fog-calculation
         */
        void setFarPlane(float farPlane) { m_FarPlane = farPlane; }

        /**
         * @return Texture to be used for the given state/layer
         */
        Handle::TextureHandle getSkyTextureOfLayer(size_t skyStateIdx, size_t layerIdx);

        /**
         * Loads the sky-texture of the given state/layer by world name
         * @return Success
         */
        bool loadSkyTextureOfLayer(size_t skyStateIdx, size_t layerIdx, const std::string& worldname);

        /**
         * @return Dome-Mesh, set up with the correct textures for the current time.
         *         Attention: Might return an invalid handle, if the game does not have any dome-mesh (Like G1)
         */
        const std::array<Handle::MeshHandle, NUM_SKY_LAYERS>& getDomeMeshes();

        /**
         * @return Mesh for the fancy looking glow effect in the back of the skydome
         */
        Handle::MeshHandle getSkyDomeColorLayerMesh() { return m_DomeColorLayerMesh; }

        /**
         * Fills the given two color-values with colors to be interpolated from top to bottom of
         * the color-dome-mesh
         */
        void getDomeColors(Math::float3& color0, Math::float3& color1);
    private:

        /**
         * Loads the config describing the skycolors
         */
        void loadSkyConfig();

        /**
         * Puts the colors from a sky-config into the sky-states
         */
        void fillColorsFromSkyConfig(const SkyConfig& config);

        /**
         * Reloads all textures used by the states/layers, depending on the given world name
         */
        void reloadAllSkyTextures(const std::string& worldName);

        /**
         * Loads the Dome mesh, if it can find one
         */
        void loadDomeLayerMeshes();

        /**
         * @return Filename of the mesh to use for the given layer
         */
        std::string getDomeLayerMeshFileName(size_t layerIdx);

        /**
         * Assigns the correct textures for the current time to the dome-mesh
         */
        void setupDomeMeshTexturesForCurrentTime();

        /**
         * Base names have the following format: SOMETAG_LAYERx_Ay.TGA.
         * Depending on the world, textures like SOMETAG_WORLDNAME_LAYERx_Ay.TGA exist.
         */
        std::string insertWorldNameIntoSkyTextureBase(const std::string& skyTextureBase, const std::string& worldName);

        /**
         * LUT-Calculation how the original engine does it
         * @param col0 Not sure, was set to (0,0,0) in the original
         * @param col1 Main sky color?
         * @param pLut Pointer to an array of 256 Math::float4 values
         */
        void calculateLUT_ZenGin(const Math::float3& col0, const Math::float3& col1, Math::float4* pLut);

        /**
         * Array containing the currently calculated lighting values
         * Note: uint32_t would be enough, but these are hard to handle on the GPU
         */
        Math::float4 m_LUT[256];

        /**
         * Skystates we're interpolating
         */
        std::array<SkyState, ESkyPresetType::ESPT_NUM_PRESETS> m_SkyStates;

        /**
         * Interpolated skystate
         */
        SkyState m_MasterState;

        /**
         * World this sky belongs to
         */
        World::WorldInstance& m_World;

        /**
         * Global Farplane
         */
        float m_FarPlane;

        /**
         * Dome-Mesh for rendering the sky. Might be invalid if no dome-mesh can be found in the gamedata (Like in G1)
         */
        std::array<Handle::MeshHandle, NUM_SKY_LAYERS> m_DomeMeshesByLayer;
        Handle::MeshHandle m_DomeColorLayerMesh;
    };
}
