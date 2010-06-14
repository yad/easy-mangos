#include "Player.h"
#include "PlayerAI.h"
#include "PlayerAIMgr.h"
#include "Chat.h"
#include "Language.h"

PlayerAIMgr::PlayerAIMgr(Player* const player) : m_player(player)
{
}

PlayerAIMgr::~PlayerAIMgr() 
{
}

void PlayerAIMgr::UpdateAI(const uint32 p_time) {}


bool ChatHandler::HandlePlayerAI(const char* args)
{
    if (!*args)
        return false;

    Player* chr = m_session->GetPlayer();

    if (strncmp(args, "on", 3) == 0)
    {
        PlayerAI* ai = new PlayerAI(chr);
        if(ai)
        {
            chr->SetPlayerAI(ai);
            ai = chr->GetPlayerAI();
            if(ai)
            {
                ai->SetEnable(true);
                SendSysMessage("PlayerAI Mode ON");
                return true;
            }
        }
    }
    else if (strncmp(args, "off", 4) == 0)
    {
        PlayerAI* ai = chr->GetPlayerAI();
        if(ai)
        {
            ai->SetEnable(false);
            SendSysMessage("PlayerAI Mode OFF");
            return true;
        }
    }

    SendSysMessage(LANG_USE_BOL);
    return false;
}