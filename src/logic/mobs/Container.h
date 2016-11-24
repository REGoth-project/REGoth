//
// Created by desktop on 06.10.16.
//

#pragma once
#include "../MobController.h"

namespace Logic
{
    namespace MobCores
    {
        class Container : public MobCore
        {
        public:
            Container(World::WorldInstance& world, const Handle::EntityHandle& entity);
            virtual ~Container();

            /**
             * Creates the contents of this container from the string found inside the zen-file
             * @param contentString String containing the items in format "item:num, ..."
             */
            void createContents(const std::string& contentString);

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

            struct Item
            {
                std::string instance;
                unsigned int count;
            };

            /**
             * Items stored inside this container
             */
            std::vector<Item> m_Contents;
        };

    }
}






