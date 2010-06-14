#ifndef _PLAYERAIMGR_H
#define _PLAYERAIMGR_H

class MANGOS_DLL_SPEC PlayerAIMgr
{
    public:
        PlayerAIMgr(Player* const player);
        virtual ~PlayerAIMgr();

        void UpdateAI(const uint32 p_time);

        PlayerAIMgr* GetPlayerAIMgr() { return this; };

    private:
        Player* const m_player;
};

#endif
