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
        Unit* FindNewVictim();
        void MovementUpdate();
        bool IsMoving();
        void SetEnable(bool b) {m_enable = b; };
        bool IsEnable() {return m_enable; };
        float GetPositionX() { return m_position_x; };
        float GetPositionY() { return m_position_y; };
        void SetPositionX(float x) { m_position_x = x; };
        void SetPositionY(float y) { m_position_y = y; };
        void CalculateXY(float &x, float &y);
        void DoLoot();

    private:
        Player* const m_player;
        Unit* m_victim;
        time_t m_ignoreAIUpdatesUntilTime;
        bool m_enable;
        bool m_need_to_cure;
        float m_position_x, m_position_y;
};

#endif
