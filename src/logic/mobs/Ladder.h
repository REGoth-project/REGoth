//
// Created by desktop on 06.10.16.
//

#pragma once
#include "../MobController.h"

namespace Logic
{
    namespace MobCores
    {
        class Ladder : public MobCore
        {
        public:
            Ladder(World::WorldInstance& world, const Handle::EntityHandle& entity);

            ~Ladder() override = default;

            /**
             *
             * @param npc
             * @param from Current state
             * @param to State to go to
             */
            void onBeginStateChange(Handle::EntityHandle npc, int from, int to) override;

            /**
             *
             * @param npc
             * @param from Current state
             * @param to State to go to
             */
            void onEndStateChange(Handle::EntityHandle npc, int from, int to) override;

        private:
        };
    }
}
