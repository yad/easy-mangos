#include "PlayerbotClassAI.h"
#include "Common.h"

PlayerbotClassAI::PlayerbotClassAI(Player* const bot, PlayerbotAI* const ai): m_bot(bot), m_ai(ai) {}
PlayerbotClassAI::~PlayerbotClassAI() {}

void PlayerbotClassAI::InitSpells(PlayerbotAI* const ai){}

bool PlayerbotClassAI::DoFirstCombatManeuver(Unit *){return false;}
void PlayerbotClassAI::DoNextCombatManeuver(Unit *) {}
bool PlayerbotClassAI::DoNonCombatActions(){return false;}

void PlayerbotClassAI::BuffPlayer(Player* target) {}

