#include "PlayerbotClassAI.h"
#include "Common.h"

PlayerbotClassAI::PlayerbotClassAI(Player* const bot, PlayerbotAI* const ai): m_bot(bot), m_ai(ai) {}
PlayerbotClassAI::~PlayerbotClassAI() {}

void PlayerbotClassAI::InitSpells(PlayerbotAI* const ai){}

bool PlayerbotClassAI::DoFirstCombatManeuver(Unit *)
{
    // return false, if done with opening moves/spells
    return false;
}
void PlayerbotClassAI::DoNextCombatManeuver(Unit *) {}

void PlayerbotClassAI::DoNonCombatActions(){}

void PlayerbotClassAI::BuffPlayer(Player* target) {}

