/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PlayerbotClassAI.h"
#include "Common.h"

PlayerbotClassAI::PlayerbotClassAI(Player* const bot, PlayerbotAI* const ai): m_bot(bot), m_ai(ai) {}
PlayerbotClassAI::~PlayerbotClassAI() {}

bool PlayerbotClassAI::DoCombatManeuver(Unit *, bool) { return false; }
void PlayerbotClassAI::DoFastBuffOnOneself() {}
bool PlayerbotClassAI::DoEvadeAction() { return false; }
bool PlayerbotClassAI::DoProtectSelfAction() { return false; }

void PlayerbotClassAI::DoNonCombatActions(){}

bool PlayerbotClassAI::BuffPlayer(Player* target) { return false; }

void PlayerbotClassAI::InitSpells(PlayerbotAI* const ai) {}
void PlayerbotClassAI::ReinitCycles() {};