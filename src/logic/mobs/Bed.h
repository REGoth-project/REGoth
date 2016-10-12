//
// Created by desktop on 06.10.16.
//

#pragma once
#include "../MobController.h"

namespace Logic
{
    namespace MobCores
    {
        class Bed : public MobCore
        {
        public:
            Bed(World::WorldInstance& world, const Handle::EntityHandle& entity);
            virtual ~Bed();

            std::string getSchemeName() override;
            void onFreePositionFound(Handle::EntityHandle npc, InteractPosition* pos) override;

        private:

            // Used to set the sceme to either front or back
            bool m_EnteringBackside;
        };

    }
}




