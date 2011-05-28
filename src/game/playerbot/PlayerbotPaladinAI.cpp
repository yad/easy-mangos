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

#include "PlayerbotPaladinAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotPaladinAI::PlayerbotPaladinAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

void PlayerbotPaladinAI::InitSpells(PlayerbotAI* const ai)
{
    RETRIBUTION_AURA              = ai->initSpell(RETRIBUTION_AURA_1);
    CRUSADER_AURA                 = ai->initSpell(CRUSADER_AURA_1);
    CRUSADER_STRIKE               = ai->initSpell(CRUSADER_STRIKE_1);
    SEAL_OF_COMMAND               = ai->initSpell(SEAL_OF_COMMAND_1);
    JUDGEMENT_OF_LIGHT            = ai->initSpell(JUDGEMENT_OF_LIGHT_1);
    JUDGEMENT_OF_WISDOM           = ai->initSpell(JUDGEMENT_OF_WISDOM_1);
    JUDGEMENT_OF_JUSTICE          = ai->initSpell(JUDGEMENT_OF_JUSTICE_1);
    DIVINE_STORM                  = ai->initSpell(DIVINE_STORM_1);
    BLESSING_OF_MIGHT             = ai->initSpell(BLESSING_OF_MIGHT_1);
    GREATER_BLESSING_OF_MIGHT     = ai->initSpell(GREATER_BLESSING_OF_MIGHT_1);
    HAMMER_OF_WRATH               = ai->initSpell(HAMMER_OF_WRATH_1);
    FLASH_OF_LIGHT                = ai->initSpell(FLASH_OF_LIGHT_1); // Holy
    HOLY_LIGHT                    = ai->initSpell(HOLY_LIGHT_1);
    HOLY_SHOCK                    = ai->initSpell(HOLY_SHOCK_1);
    HOLY_WRATH                    = ai->initSpell(HOLY_WRATH_1);
    DIVINE_FAVOR                  = ai->initSpell(DIVINE_FAVOR_1);
    CONCENTRATION_AURA            = ai->initSpell(CONCENTRATION_AURA_1);
    BLESSING_OF_WISDOM            = ai->initSpell(BLESSING_OF_WISDOM_1);
    GREATER_BLESSING_OF_WISDOM    = ai->initSpell(GREATER_BLESSING_OF_WISDOM_1);
    CONSECRATION                  = ai->initSpell(CONSECRATION_1);
    AVENGING_WRATH                = ai->initSpell(AVENGING_WRATH_1);
    LAY_ON_HANDS                  = ai->initSpell(LAY_ON_HANDS_1);
    EXORCISM                      = ai->initSpell(EXORCISM_1);
    SACRED_SHIELD                 = ai->initSpell(SACRED_SHIELD_1);
    DIVINE_PLEA                   = ai->initSpell(DIVINE_PLEA_1);
    BLESSING_OF_KINGS             = ai->initSpell(BLESSING_OF_KINGS_1);
    GREATER_BLESSING_OF_KINGS     = ai->initSpell(GREATER_BLESSING_OF_KINGS_1);
    BLESSING_OF_SANCTUARY         = ai->initSpell(BLESSING_OF_SANCTUARY_1);
    GREATER_BLESSING_OF_SANCTUARY = ai->initSpell(GREATER_BLESSING_OF_SANCTUARY_1);
    HAMMER_OF_JUSTICE             = ai->initSpell(HAMMER_OF_JUSTICE_1);
    RIGHTEOUS_FURY                = ai->initSpell(RIGHTEOUS_FURY_1);
    RIGHTEOUS_DEFENSE             = ai->initSpell(RIGHTEOUS_DEFENSE_1);
    SHADOW_RESISTANCE_AURA        = ai->initSpell(SHADOW_RESISTANCE_AURA_1);
    DEVOTION_AURA                 = ai->initSpell(DEVOTION_AURA_1);
    FIRE_RESISTANCE_AURA          = ai->initSpell(FIRE_RESISTANCE_AURA_1);
    FROST_RESISTANCE_AURA         = ai->initSpell(FROST_RESISTANCE_AURA_1);
    HAND_OF_PROTECTION            = ai->initSpell(HAND_OF_PROTECTION_1);
    DIVINE_PROTECTION             = ai->initSpell(DIVINE_PROTECTION_1);
    DIVINE_INTERVENTION           = ai->initSpell(DIVINE_INTERVENTION_1);
    DIVINE_SACRIFICE              = ai->initSpell(DIVINE_SACRIFICE_1);
    DIVINE_SHIELD                 = ai->initSpell(DIVINE_SHIELD_1);
    HOLY_SHIELD                   = ai->initSpell(HOLY_SHIELD_1);
    AVENGERS_SHIELD               = ai->initSpell(AVENGERS_SHIELD_1);
    HAND_OF_SACRIFICE             = ai->initSpell(HAND_OF_SACRIFICE_1);
    SHIELD_OF_RIGHTEOUSNESS       = ai->initSpell(SHIELD_OF_RIGHTEOUSNESS_1);
    REDEMPTION                    = ai->initSpell(REDEMPTION_1);

    // Warrior auras
    DEFENSIVE_STANCE              = 71;   //Def Stance
    BERSERKER_STANCE              = 2458; //Ber Stance
    BATTLE_STANCE                 = 2457; //Bat Stance

    FORBEARANCE                   = 25771; // cannot be protected

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_PALADIN); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    EVERY_MAN_FOR_HIMSELF         = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
}

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}

bool PlayerbotPaladinAI::HealTarget(Unit *target)
{
    PlayerbotAI* ai = GetAI();
    uint8 hp = target->GetHealth() * 100 / target->GetMaxHealth();

    if (hp < 25 && ai->CastSpell(LAY_ON_HANDS, target))
        return true;

    if (hp < 30 && ai->CastSpell(FLASH_OF_LIGHT, target))
        return true;

    if (hp < 35 && ai->CastSpell(HOLY_SHOCK, target))
        return true;

    if (hp < 40 && ai->CastSpell(HOLY_LIGHT, target))
        return true;

    return false;
} // end HealTarget

void PlayerbotPaladinAI::DoNextCombatManeuver(Unit *pTarget)
{
    Unit* pVictim = pTarget->getVictim();

    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetLeader();
    if (!m_master)
        return;

    // damage spells
    ai->SetInFront(pTarget);
    Group *m_group = m_bot->GetGroup();
    float dist = m_bot->GetDistance(pTarget);


    //Shield master if low hp.
    uint32 masterHP = m_master->GetHealth() * 100 / m_master->GetMaxHealth();

    if (m_master->isAlive())
        if (masterHP < 25 && HAND_OF_PROTECTION > 0 && !m_master->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !m_master->HasAura(HAND_OF_PROTECTION, EFFECT_INDEX_0) && !m_master->HasAura(DIVINE_PROTECTION, EFFECT_INDEX_0) && !m_master->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0))
            ai->CastSpell(HAND_OF_PROTECTION, m_master);

    // heal group inside combat, but do not heal if tank
    if (m_group && pVictim != m_bot)  // possible tank
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            if (memberHP < 40 && ai->GetManaPercent() >= 40)  // do not heal bots without plenty of mana for master & self
                if (HealTarget(m_groupMember))
                    return;
        }
    }

    if (RIGHTEOUS_FURY > 0 && !m_bot->HasAura(RIGHTEOUS_FURY, EFFECT_INDEX_0))
        ai->CastSpell (RIGHTEOUS_FURY, m_bot);

    switch (pTarget->getClass())
    {
    case CLASS_WARLOCK:
    case CLASS_PRIEST:
        if (!m_bot->HasAura(SHADOW_RESISTANCE_AURA) && ai->CastSpell(SHADOW_RESISTANCE_AURA))
            return;
        break;
    case CLASS_MAGE:
        if (pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            if (((Player*)pTarget)->getRole() == MageFire && !m_bot->HasAura(FIRE_RESISTANCE_AURA) && ai->CastSpell(FIRE_RESISTANCE_AURA))
                return;
            else if (((Player*)pTarget)->getRole() == MageFrost && !m_bot->HasAura(FROST_RESISTANCE_AURA) && ai->CastSpell(FROST_RESISTANCE_AURA))
                return;
        }
        else if (!m_bot->HasAura(FIRE_RESISTANCE_AURA) && ai->CastSpell(FIRE_RESISTANCE_AURA))
            return;
        break;
    case CLASS_HUNTER:
        if (!m_bot->HasAura(RETRIBUTION_AURA) && ai->CastSpell(RETRIBUTION_AURA))
            return;
        break;
    default:
        if (!m_bot->HasAura(DEVOTION_AURA) && ai->CastSpell(DEVOTION_AURA))
            return;
    }

    if (ai->GetHealthPercent() <= 40 || m_master->GetHealth() <= m_master->GetMaxHealth() * 0.4)
        SpellSequence = Healing;
    else
        SpellSequence = Combat;

    switch (SpellSequence)
    {
        case Combat:
            if (CombatCounter < 1 && ai->GetManaPercent() >= 5)
            {
                if(m_bot->GetGroup())
                {
                    if (!pTarget->HasAura(JUDGEMENT_OF_LIGHT) && (ai->GetManaPercent() > 30 || pTarget->HasAura(JUDGEMENT_OF_WISDOM)) && ai->CastSpell(JUDGEMENT_OF_LIGHT, pTarget))
                        return;
                    else if (!pTarget->HasAura(JUDGEMENT_OF_WISDOM) && ai->CastSpell(JUDGEMENT_OF_WISDOM, pTarget))
                        return;
                    else if (!pTarget->HasAura(JUDGEMENT_OF_JUSTICE) && ai->CastSpell(JUDGEMENT_OF_JUSTICE, pTarget))
                        return;
                    else if (ai->CastSpell(JUDGEMENT_OF_LIGHT, pTarget))
                        return;
                }
                else
                {
                    if (!pTarget->HasAura(JUDGEMENT_OF_LIGHT) && ai->GetManaPercent() > 30 && ai->CastSpell(JUDGEMENT_OF_LIGHT, pTarget))
                        return;
                    else if (!pTarget->HasAura(JUDGEMENT_OF_WISDOM) && ai->GetManaPercent() <= 30 && ai->CastSpell (JUDGEMENT_OF_WISDOM, pTarget))
                        return;
                }

                CombatCounter++;
                break;
            }
            else if (SEAL_OF_COMMAND > 0 && !m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0) && CombatCounter < 2 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell (SEAL_OF_COMMAND, m_bot);

                CombatCounter++;
                break;
            }
            else if (HAMMER_OF_JUSTICE > 0 && !pTarget->HasAura(HAMMER_OF_JUSTICE, EFFECT_INDEX_0) && CombatCounter < 3 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell (HAMMER_OF_JUSTICE, pTarget);

                CombatCounter++;
                break;
            }
            else if (CRUSADER_STRIKE > 0 && CombatCounter < 4 && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell (CRUSADER_STRIKE, pTarget);

                CombatCounter++;
                break;
            }
            else if (AVENGING_WRATH > 0 && CombatCounter < 5 && !m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell (AVENGING_WRATH, m_bot);

                CombatCounter++;
                break;
            }
            else if (SACRED_SHIELD > 0 && CombatCounter < 6 && pVictim == m_bot && ai->GetHealthPercent() < 70 && !m_bot->HasAura(SACRED_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell (SACRED_SHIELD, m_bot);

                CombatCounter++;
                break;
            }
            else if (DIVINE_STORM > 0 && CombatCounter < 7 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell (DIVINE_STORM, pTarget);

                CombatCounter++;
                break;
            }
            else if (HAMMER_OF_WRATH > 0 && CombatCounter < 8 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell (HAMMER_OF_WRATH, pTarget);

                CombatCounter++;
                break;
            }
            else if (HOLY_WRATH > 0 && CombatCounter < 9 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell (HOLY_WRATH, pTarget);

                CombatCounter++;
                break;
            }
            else if (HAND_OF_SACRIFICE > 0 && pVictim == m_master && !m_master->HasAura(HAND_OF_SACRIFICE, EFFECT_INDEX_0) && CombatCounter < 10 && ai->GetManaPercent() >= 6)
            {
                ai->CastSpell (HAND_OF_SACRIFICE, m_master);

                CombatCounter++;
                break;
            }
            else if (DIVINE_PROTECTION > 0 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && ai->GetHealthPercent() < 30 && CombatCounter < 11 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell (DIVINE_PROTECTION, m_bot);

                CombatCounter++;
                break;
            }
            else if (RIGHTEOUS_DEFENSE > 0 && pVictim != m_bot && ai->GetHealthPercent() > 70 && CombatCounter < 12)
            {
                ai->CastSpell (RIGHTEOUS_DEFENSE, pTarget);

                CombatCounter++;
                break;
            }
            else if (DIVINE_PLEA > 0 && !m_bot->HasAura(DIVINE_PLEA, EFFECT_INDEX_0) && ai->GetManaPercent() < 50 && CombatCounter < 13)
            {
                ai->CastSpell (DIVINE_PLEA, m_bot);

                CombatCounter++;
                break;
            }
            else if (DIVINE_FAVOR > 0 && !m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0) && CombatCounter < 14)
            {
                ai->CastSpell (DIVINE_FAVOR, m_bot);

                CombatCounter++;
                break;
            }
            else if (CombatCounter > 15)
            {
                CombatCounter = 0;

                break;
            }
            else
            {
                CombatCounter = 0;

                break;
            }

        case Healing:
            if (ai->GetHealthPercent() <= 40)
            {
                HealTarget (m_bot);

                break;
            }
            if (masterHP <= 40)
            {
                HealTarget (m_master);

                break;
            }
            else
            {
                CombatCounter = 0;

                break;
            }
    }


    if (AVENGING_WRATH > 0 && !m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
        ai->CastSpell(AVENGING_WRATH, m_bot);

    if (DIVINE_SHIELD > 0 && ai->GetHealthPercent() < 30 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !m_bot->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 3)
        ai->CastSpell(DIVINE_SHIELD, m_bot);

    if (DIVINE_SACRIFICE > 0 && ai->GetHealthPercent() > 50 && pVictim != m_bot && !m_bot->HasAura(DIVINE_SACRIFICE, EFFECT_INDEX_0))
        ai->CastSpell(DIVINE_SACRIFICE, m_bot);
}

void PlayerbotPaladinAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Group* m_group = m_bot->GetGroup();
    GroupReference *ref = (m_group) ? m_group->GetFirstMember() : NULL;
    do
    {
        Player *g_member = (ref) ? ref->getSource() : m_bot;

        if (!g_member)
            continue;

        if (!g_member->isAlive())
        {
            if (ai->CastSpell(REDEMPTION, g_member))
            {
                std::string msg = "Resurrecting ";
                msg += g_member->GetName();
                m_bot->Say(msg, LANG_UNIVERSAL);
                return;
            }
        }
        else if (HealTarget(g_member))
            return;
        else if (BuffPlayer(g_member))
            return;    

    }while(ref = (ref) ? ref->next() : NULL);
}

bool PlayerbotPaladinAI::BuffPlayer(Player* target)
{
    PlayerbotAI * ai = GetAI();
    uint8 SPELL_BLESSING = 2; // See SpellSpecific enum in SpellMgr.h

    Pet * pet = target->GetPet();
    bool petCanBeBlessed = false;
    if (pet)
        petCanBeBlessed = ai->CanReceiveSpecificSpell(SPELL_BLESSING, pet);

    if (!ai->CanReceiveSpecificSpell(SPELL_BLESSING, target) && !petCanBeBlessed)
        return false;

    switch (target->getClass())
    {
        case CLASS_DRUID:
        case CLASS_SHAMAN:
        case CLASS_PALADIN:
            if (Bless(BLESSING_OF_MIGHT, target))
                return true;
            if (Bless(BLESSING_OF_KINGS, target))
                return true;
            if (Bless(BLESSING_OF_WISDOM, target))
                return true;
            if (Bless(BLESSING_OF_SANCTUARY, target))
                return true;
            else
                return false;
        case CLASS_DEATH_KNIGHT:
        case CLASS_HUNTER:
            if (petCanBeBlessed)
                if (Bless(BLESSING_OF_MIGHT, pet))
                    return true;
                if (Bless(BLESSING_OF_KINGS, pet))
                    return true;
                if (Bless(BLESSING_OF_SANCTUARY, pet))
                    return true;
        case CLASS_ROGUE:
        case CLASS_WARRIOR:
            if (Bless(BLESSING_OF_MIGHT, target))
                return true;
            if (Bless(BLESSING_OF_KINGS, target))
                return true;
            if (Bless(BLESSING_OF_SANCTUARY, target))
                return true;
            else
                return false;
        case CLASS_WARLOCK:
            if (petCanBeBlessed)
            {
                if (pet->getPowerType() == POWER_MANA)
                {
                    if (Bless(BLESSING_OF_WISDOM, pet))
                        return true;
                }
                else
                {
                    if (Bless(BLESSING_OF_MIGHT, pet))
                        return true;
                }
                if (Bless(BLESSING_OF_KINGS, pet))
                    return true;
                if (Bless(BLESSING_OF_SANCTUARY, pet))
                    return true;
            }
        case CLASS_PRIEST:
        case CLASS_MAGE:
            if (Bless(BLESSING_OF_WISDOM, target))
                return true;
            if (Bless(BLESSING_OF_KINGS, target))
                return true;
            if (Bless(BLESSING_OF_SANCTUARY, target))
                return true;
            else
                return false;
    }
    return false;
}

bool PlayerbotPaladinAI::Bless(uint32 spellId, Unit *target)
{
    if (spellId == 0)
        return false;

    PlayerbotAI * ai = GetAI();

    if (spellId == BLESSING_OF_MIGHT)
    {
        if (GREATER_BLESSING_OF_MIGHT && ai->HasSpellReagents(GREATER_BLESSING_OF_MIGHT) && ai->Buff(GREATER_BLESSING_OF_MIGHT, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_WISDOM)
    {
        if (GREATER_BLESSING_OF_WISDOM && ai->HasSpellReagents(GREATER_BLESSING_OF_WISDOM) && ai->Buff(GREATER_BLESSING_OF_WISDOM, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_KINGS)
    {
        if (GREATER_BLESSING_OF_KINGS && ai->HasSpellReagents(GREATER_BLESSING_OF_KINGS) && ai->Buff(GREATER_BLESSING_OF_KINGS, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_SANCTUARY)
    {
        if (GREATER_BLESSING_OF_SANCTUARY && ai->HasSpellReagents(GREATER_BLESSING_OF_SANCTUARY) && ai->Buff(GREATER_BLESSING_OF_SANCTUARY, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }

    // Should not happen, but let it be here
    return false;
}
