#ifndef _PLAYERAI_H
#define _PLAYERAI_H

class MANGOS_DLL_SPEC PlayerAI
{
    public:
        PlayerAI(Player* const player);
        virtual ~PlayerAI();

        void UpdateAI(const uint32 p_time);

        Player* GetPlayer() const { return m_player; };
        void SetVictim(Unit* victim) {m_victim = victim; };
        Unit* GetVictim() {return m_victim; };
        Unit* FindNewVictim(Player* const player);
        void MovementUpdate(Player* const player);
        void SetEnable(bool b) {m_enable = b; };
        bool IsEnable() {return m_enable; };

    private:
        Player* const m_player;
        Unit* m_victim;
        time_t m_ignoreAIUpdatesUntilTime;
        bool m_enable;
};

#endif
