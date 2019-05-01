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

            ~Container() override = default;

            struct Item
            {
                std::string instance;
                unsigned int count;
            };

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
            void onBeginStateChange(Handle::EntityHandle npc, int from, int to) override;

            /**
             *
             * @param npc
             * @param from Current state
             * @param to State to go to
             */
            void onEndStateChange(Handle::EntityHandle npc, int from, int to) override;

            /**
             * Saves this cores settings to the given JSON-object
             */
            void exportCore(json& j) override;
            void importCore(const json& j) override;

            /**
             * Checks how many of the given items we got in here
             * @param instance Item to check for
             * @return Number of items of the given instance
             */
            unsigned int getNumItemsOf(const std::string& instance) const;

            /**
             * @return
             */
            const std::vector<Item>& getContents() { return m_Contents; }
        private:
            /**
             * Items stored inside this container
             */
            std::vector<Item> m_Contents;
        };
    }
}
