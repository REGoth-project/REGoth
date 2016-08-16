#pragma once

#include <handle/HandleDef.h>
#include "Texture.h"

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

    class Sky
    {
    public:

        /**
         * There is one sky and one clouds-layer
         */
        enum { NUM_SKY_LAYERS = 2 };
        struct SkyLayerData
        {
            // This layers texture
            Handle::TextureHandle texture;

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
            float			time;

            // Color values
            Math::float3	baseColor;
            Math::float3	fogColor;
            Math::float3	domeColorUpper;

            // Base-fog distance modifier
            float			fogDistance;

            // Whether the sun should be active. If false, the moon is.
            bool			sunEnabled;

            // Information about the cloud and sky layers
            SkyLayerData	layers[NUM_SKY_LAYERS];
        };

        Sky(World::WorldInstance& world);
        ~Sky();

        /**
         * Updates the sky and the colors
         * @param deltaTime Time since last frame
         */
        void interpolate(double deltaTime);

        /**
         * Initializes the given skystate to the given type
         * @param type Type of state we want to make
         * @param s state to fill with values
         * @param texAlloc Texture allocator to take the data from
         */
        static void initSkyState(ESkyPresetType type, SkyState& s, Textures::TextureAllocator& texAlloc);

        /**
         * Fills the m_SkyStates with their preset values
         */
        void fillSkyStates();

        /**
         * Returns the current LUT-array. Each entry of this array maps an 8-bit luminance to another color.
         * @return Pointer to the LUT
         */
        const Math::float4* getLUTPtr(){ return m_LUT; }

        /**
         * @return Time of day.
         */
        float getTimeOfDay(){ return m_MasterState.time; }

        /**
         * @return current interpolated sky-state
         */
        const SkyState& getMasterState(){ return m_MasterState; }

        /**
         * Calculates the near- and farplanes for the fog
         * @param cameraWorld current cameras world position
         * @param near target for nearplane
         * @param far target for farplane
         */
        void getFogValues(const Math::float3& cameraWorld, float& near, float& far, Math::float3& fogColor);

        /**
         * Sets the farplane, needed to do the fog-calculation
         */
        void setFarPlane(float far){ m_FarPlane = far; }
    private:

        /**
         * LUT-Calculation how the original engine does it
         * @param col0 Not sure, was set to (0,0,0) in the original
         * @param col1 Main sky color?
         * @param pLut Pointer to an array of 256 Math::float4 values
         */
        void calculateLUT_ZenGin(const Math::float3 &col0, const Math::float3 &col1, Math::float4 *pLut);

        /**
         * Array containing the currently calculated lighting values
         * Note: uint32_t would be enough, but these are hard to handle on the GPU
         */
        Math::float4 m_LUT[256];

        /**
         * Skystates we're interpolating
         */
        SkyState m_SkyStates[ESkyPresetType::ESPT_NUM_PRESETS];

        /**
         * Interpolated skystate
         */
        SkyState m_MasterState;
        double m_MasterTime;

        /**
         * World this sky belongs to
         */
        World::WorldInstance& m_World;

        /**
         * Global Farplane
         */
        float m_FarPlane;
    };
}
