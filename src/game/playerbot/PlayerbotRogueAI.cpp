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

#include "PlayerbotRogueAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotRogueAI::PlayerbotRogueAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

void PlayerbotRogueAI::InitSpells(PlayerbotAI* const ai)
{
    SINISTER_STRIKE          = ai->initSpell(SINISTER_STRIKE_1);
    BACKSTAB                 = ai->initSpell(BACKSTAB_1);
    KICK                     = ai->initSpell(KICK_1);
    FEINT                    = ai->initSpell(FEINT_1);
    FAN_OF_KNIVES            = ai->initSpell(FAN_OF_KNIVES_1);
    GOUGE                    = ai->initSpell(GOUGE_1);
    SPRINT                   = ai->initSpell(SPRINT_1);

    SHADOWSTEP               = ai->initSpell(SHADOWSTEP_1);
    STEALTH                  = ai->initSpell(STEALTH_1);
    VANISH                   = ai->initSpell(VANISH_1);
    EVASION                  = ai->initSpell(EVASION_1);
    CLOAK_OF_SHADOWS         = ai->initSpell(CLOAK_OF_SHADOWS_1);
    HEMORRHAGE               = ai->initSpell(HEMORRHAGE_1);
    GHOSTLY_STRIKE           = ai->initSpell(GHOSTLY_STRIKE_1);
    SHADOW_DANCE             = ai->initSpell(SHADOW_DANCE_1);
    BLIND                    = ai->initSpell(BLIND_1);
    DISTRACT                 = ai->initSpell(DISTRACT_1);
    PREPARATION              = ai->initSpell(PREPARATION_1);
    PREMEDITATION            = ai->initSpell(PREMEDITATION_1);
    PICK_POCKET              = ai->initSpell(PICK_POCKET_1);

    EVISCERATE               = ai->initSpell(EVISCERATE_1);
    KIDNEY_SHOT              = ai->initSpell(KIDNEY_SHOT_1);
    SLICE_DICE               = ai->initSpell(SLICE_AND_DICE_1);
    GARROTE                  = ai->initSpell(GARROTE_1);
    EXPOSE_ARMOR             = ai->initSpell(EXPOSE_ARMOR_1);
    RUPTURE                  = ai->initSpell(RUPTURE_1);
    DISMANTLE                = ai->initSpell(DISMANTLE_1);
    CHEAP_SHOT               = ai->initSpell(CHEAP_SHOT_1);
    AMBUSH                   = ai->initSpell(AMBUSH_1);
    MUTILATE                 = ai->initSpell(MUTILATE_1);

    RECENTLY_BANDAGED   = 11196; // first aid check
    // racial
    ARCANE_TORRENT           = ai->initSpell(ARCANE_TORRENT_ROGUE);
    STONEFORM                = ai->initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST            = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF    = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD               = ai->initSpell(SHADOWMELD_ALL);
    BLOOD_FURY               = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    BERSERKING               = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN     = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotRogueAI::~PlayerbotRogueAI() {}

bool PlayerbotRogueAI::DoFirstCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();

    if (STEALTH > 0 && !m_bot->HasAura(STEALTH, EFFECT_INDEX_0) && ai->CastSpell(STEALTH, m_bot))
    {

        m_bot->addUnitState(UNIT_STAT_CHASE); // ensure that the bot does not use MoveChase(), as this doesn't seem to work with STEALTH

        return true;
    }
    else if (m_bot->HasAura(STEALTH, EFFECT_INDEX_0))
    {
        m_bot->GetMotionMaster()->MoveFollow(pTarget, 4.5f, m_bot->GetOrientation());
        return false;
    }
    return false;
}

void PlayerbotRogueAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!pTarget)
        return;

    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetLeader();
    if (!m_master)
        return;

    ai->SetInFront(pTarget);
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetDistance(pTarget);

    // TODO: make this work better...
    /*if (pVictim)
       {
        if( pVictim!=m_bot && !m_bot->hasUnitState(UNIT_STAT_FOLLOW) && !pTarget->isInBackInMap(m_bot,10) ) {

            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveFollow( pTarget, 1, 2*M_PI );
        }
        else if( pVictim==m_bot && m_bot->hasUnitState(UNIT_STAT_FOLLOW) )
        {

            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveChase( pTarget );
        }
       }*/

    //Rouge like behaviour. ^^
/*    if (VANISH > 0 && m_master->isDead()) { //Causes the server to crash :( removed for now.
        m_bot->AttackStop();
        m_bot->RemoveAllAttackers();
        ai->CastSpell(VANISH);
   //        m_bot->RemoveAllSpellCooldown();

    }*/

    // decide what to do:
    if (pVictim == m_bot && CLOAK_OF_SHADOWS > 0 && pVictim->HasAura(SPELL_AURA_PERIODIC_DAMAGE) && !m_bot->HasAura(CLOAK_OF_SHADOWS, EFFECT_INDEX_0) && ai->CastSpell(CLOAK_OF_SHADOWS))
    {

        return;
    }
    else if (m_bot->HasAura(STEALTH, EFFECT_INDEX_0))
    SpellSequence = RogueSpeStealth;
    else if (pTarget->IsNonMeleeSpellCasted(true))
        SpellSequence = RogueSpeSpellPreventing;
    else if (pVictim == m_bot && ai->GetHealthPercent() < 40)
        SpellSequence = RogueSpeThreat;
    else
        SpellSequence = RogueSpeCombat;

    // we fight in melee, target is not in range, skip the next part!
    if (fTargetDist > ATTACK_DISTANCE)
        return;


    switch (SpellSequence)
    {
        case RogueSpeStealth:

            /*if (PICK_POCKET > 0 && ai->CastSpell(PICK_POCKET, pTarget) && ai->PickPocket(pTarget))

        else */if (PREMEDITATION > 0 && ai->CastSpell(PREMEDITATION, pTarget))
        {}
            else if (AMBUSH > 0 && ai->GetEnergyAmount() >= 60 && ai->CastSpell(AMBUSH, pTarget))
        {}
            else if (CHEAP_SHOT > 0 && !pTarget->HasAura(CHEAP_SHOT, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(CHEAP_SHOT, pTarget))
        {}
            else if (GARROTE > 0 && ai->GetEnergyAmount() >= 50 && ai->CastSpell(GARROTE, pTarget))
        {}
            else
                m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
        break;
        case RogueSpeThreat:

            if (GOUGE > 0 && ai->GetEnergyAmount() >= 45 && !pTarget->HasAura(GOUGE, EFFECT_INDEX_0) && ai->CastSpell(GOUGE, pTarget))
        {}
            else if (EVASION > 0 && ai->GetHealthPercent() <= 35 && !m_bot->HasAura(EVASION, EFFECT_INDEX_0) && ai->CastSpell(EVASION))
        {}
            else if (BLIND > 0 && ai->GetHealthPercent() <= 30 && !pTarget->HasAura(BLIND, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 30 && ai->CastSpell(BLIND, pTarget))
        {}
            else if (FEINT > 0 && ai->GetHealthPercent() <= 25 && ai->GetEnergyAmount() >= 20 && ai->CastSpell(FEINT))
        {}
            else if (VANISH > 0 && ai->GetHealthPercent() <= 20 && !m_bot->HasAura(FEINT, EFFECT_INDEX_0) && ai->CastSpell(VANISH))
        {}
            else if (PREPARATION > 0 && ai->CastSpell(PREPARATION))
        {}
            else if (m_bot->getRace() == RACE_NIGHTELF && ai->GetHealthPercent() <= 15 && !m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0) && ai->CastSpell(SHADOWMELD, m_bot))
        {}
            break;
        case RogueSpeSpellPreventing:

            if (KIDNEY_SHOT > 0 && ai->GetEnergyAmount() >= 25 && m_bot->GetComboPoints() >= 2 && ai->CastSpell(KIDNEY_SHOT, pTarget))
        {}
            else if (KICK > 0 && ai->GetEnergyAmount() >= 25  && ai->CastSpell(KICK, pTarget))
        {}
            break;
        case RogueSpeCombat:
        default:

            if (m_bot->GetComboPoints() <= 4)
            {
                if (SHADOW_DANCE > 0 && !m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && ai->CastSpell(SHADOW_DANCE, m_bot))
        {}
                else if (CHEAP_SHOT > 0 && m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && !pTarget->HasAura(CHEAP_SHOT, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(CHEAP_SHOT, pTarget))
        {}
                else if (AMBUSH > 0 && m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(AMBUSH, pTarget))
        {}
                else if (GARROTE > 0 && m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 50 && ai->CastSpell(GARROTE, pTarget))
        {}
                else if (BACKSTAB > 0 && pTarget->isInBackInMap(m_bot, 1) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(BACKSTAB, pTarget))
        {}
                else if (MUTILATE > 0 && ai->GetEnergyAmount() >= 60 && ai->CastSpell(MUTILATE, pTarget))
        {}
                else if (SINISTER_STRIKE > 0 && ai->GetEnergyAmount() >= 45 && ai->CastSpell(SINISTER_STRIKE, pTarget))
        {}
                else if (GHOSTLY_STRIKE > 0 && ai->GetEnergyAmount() >= 40 && ai->CastSpell(GHOSTLY_STRIKE, pTarget))
        {}
                else if (HEMORRHAGE > 0 && ai->GetEnergyAmount() >= 35 && ai->CastSpell(HEMORRHAGE, pTarget))
        {}
                else if (DISMANTLE > 0 && !pTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(DISMANTLE, pTarget))
        {}
                else if (SHADOWSTEP > 0 && ai->GetEnergyAmount() >= 10 && ai->CastSpell(SHADOWSTEP, pTarget))
        {}
                else if (m_bot->getRace() == RACE_BLOODELF && !pTarget->HasAura(ARCANE_TORRENT, EFFECT_INDEX_0) && ai->CastSpell(ARCANE_TORRENT, pTarget))
        {}
                else if (m_bot->getRace() == RACE_HUMAN && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(EVERY_MAN_FOR_HIMSELF, m_bot))
        {}
                else if (m_bot->getRace() == RACE_UNDEAD && m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(WILL_OF_THE_FORSAKEN, m_bot))
        {}
                else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && ai->CastSpell(STONEFORM, m_bot))
        {}
                else if (m_bot->getRace() == RACE_GNOME && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) && ai->CastSpell(ESCAPE_ARTIST, m_bot))
        {}
                else if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0) && ai->CastSpell(BLOOD_FURY, m_bot))
        {}
                else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0) && ai->CastSpell(BERSERKING, m_bot))
        {}
            }
            else
            {
                if (EVISCERATE > 0 && pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, pTarget))
        {}
                else if (EVISCERATE > 0 && pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, pTarget))
        {}
                else if (KIDNEY_SHOT > 0 && pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 25 && ai->CastSpell(KIDNEY_SHOT, pTarget))
        {}
                else if (SLICE_DICE > 0 && pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 25 && ai->CastSpell(SLICE_DICE, pTarget))
        {}
                else if (SLICE_DICE > 0 && pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 25 && ai->CastSpell(SLICE_DICE, pTarget))
        {}
                else if (EXPOSE_ARMOR > 0 && pTarget->getClass() == CLASS_WARRIOR && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, pTarget))
        {}
                else if (EXPOSE_ARMOR > 0 && pTarget->getClass() == CLASS_PALADIN && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, pTarget))
        {}
                else if (EXPOSE_ARMOR > 0 && pTarget->getClass() == CLASS_DEATH_KNIGHT && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, pTarget))
        {}
                else if (RUPTURE > 0 && pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 25 && ai->CastSpell(RUPTURE, pTarget))
        {}
                else if (RUPTURE > 0 && pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 25 && ai->CastSpell(RUPTURE, pTarget))
        {}
                else if (EVISCERATE > 0 && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, pTarget))
        {}
            }
            break;
    }

}

// end DoNextCombatManeuver

void PlayerbotRogueAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetLeader();
    if (!m_master)
        return;

    // remove stealth
    if (m_bot->HasAura(STEALTH))
        m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

    // Search and apply poisons to weapons
    // Mainhand ...
    Item * poison, * weapon;
    weapon = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        poison = ai->FindConsumable(INSTANT_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(WOUND_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(DEADLY_POISON_DISPLAYID);
        if (poison)
        {
            ai->UseItem(poison, EQUIPMENT_SLOT_MAINHAND);
            ai->SetIgnoreUpdateTime(5);
        }
    }
    //... and offhand
    weapon = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        poison = ai->FindConsumable(DEADLY_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(WOUND_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(INSTANT_POISON_DISPLAYID);
        if (poison)
        {
            ai->UseItem(poison, EQUIPMENT_SLOT_OFFHAND);
            ai->SetIgnoreUpdateTime(5);
        }
    }

} // end DoNonCombatActions
