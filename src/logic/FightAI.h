#pragma once

#include <daedalus/DaedalusGameState.h>
#include <daedalus/DaedalusVM.h>
#include <handle/HandleDef.h>
#include <memory>
#include <vector>

namespace Daedalus
{
    class DaedalusVM;
}

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class FightAI
    {
    public:
        /**
         * Values found inside Fight.d
         */
        enum class Kind
        {
            MOVE_NONE = 0,
            MOVE_RUN = 1,
            MOVE_RUNBACK = 2,
            MOVE_JUMPBACK = 3,
            MOVE_TURN = 4,
            MOVE_STRAFE = 5,
            MOVE_ATTACK = 6,
            MOVE_SIDEATTACK = 7,
            MOVE_FRONTATTACK = 8,
            MOVE_TRIPLEATTACK = 9,
            MOVE_WHIRLATTACK = 10,
            MOVE_MASTERATTACK = 11,
            MOVE_TURNTOHIT = 15,
            MOVE_PARADE = 17,
            MOVE_STANDUP = 18,
            MOVE_WAIT = 19,
        };

        FightAI(World::WorldInstance& world);
        virtual ~FightAI();

        /**
         * Searches for all instances with the parent C_FightAI and writes them to the log.
         */
        std::vector<std::string> dumpAllMoveInstancesToLog();

    private:

        /**
         * @return The absolute path of where FIGHT.DAT is located at.
         */
        std::string findFightDat();

        /**
         * @return Tries to load FIGHT.DAT. Returns nullptr if failed.
         */
        std::unique_ptr<Daedalus::DaedalusVM> loadFightDat();

        World::WorldInstance& m_World;
    };
}  // namespace Logic
