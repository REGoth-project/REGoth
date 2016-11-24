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
            virtual ~Ladder();

            /**
             *
             * @param npc
             * @param from Current state
             * @param to State to go to
             */
            virtual void onBeginStateChange(Handle::EntityHandle npc, int from, int to);

            /**
             *
             * @param npc
             * @param from Current state
             * @param to State to go to
             */
            virtual void onEndStateChange(Handle::EntityHandle npc, int from, int to);

        private:

        };

    }
}




