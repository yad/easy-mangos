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

bool PlayerbotRogueAI::DoFirstCombatManeuver(Unit* pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    if (!ai->HasAura(STEALTH, m_bot) && ai->CastSpell(STEALTH, m_bot))
        return true;

    return true;
}

void PlayerbotRogueAI::DoNextCombatManeuver(Unit* pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    if (!pTarget)
        return;

    /*switch (ai->GetScenarioType())
    {
        //SCENARIO_PVEEASY,
        //SCENARIO_PVEHARD,
        //SCENARIO_DUEL,
        //SCENARIO_PVPEASY,
        //SCENARIO_PVPHARD
        //return;
    }*/

    ai->SetInFront(pTarget);
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetDistance(pTarget);

    // TODO: make this work better...
    /*if (pVictim)
       {
        if( pVictim!=m_bot && !m_bot->hasUnitState(UNIT_STAT_FOLLOW) && !pTarget->isInBackInMap(m_bot,10) ) {
            ai->TellMaster( "getting behind target" );
            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveFollow( pTarget, 1, 2*M_PI );
        }
        else if( pVictim==m_bot && m_bot->hasUnitState(UNIT_STAT_FOLLOW) )
        {
            ai->TellMaster( "chasing attacking target" );
            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveChase( pTarget );
        }
       }*/

    //Rouge like behaviour. ^^
/*    if (m_master->isDead()) { //Causes the server to crash :( removed for now.
        m_bot->AttackStop();
        m_bot->RemoveAllAttackers();
        ai->CastSpell(VANISH);
   //        m_bot->RemoveAllSpellCooldown();
        ai->TellMaster("AttackStop, CombatStop, Vanish");
    }*/

    // decide what to do:
    if (pVictim == m_bot && pVictim->HasAura(SPELL_AURA_PERIODIC_DAMAGE) && !ai->HasAura(CLOAK_OF_SHADOWS, m_bot) && ai->CastSpell(CLOAK_OF_SHADOWS))
        return;
    else if (ai->HasAura(STEALTH, m_bot))
        SpellSequence = RogueSpeStealth;
    else if (pTarget->IsNonMeleeSpellCasted(true))
        SpellSequence = RogueSpellPreventing;
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
            if (ai->CastSpell(PREMEDITATION, pTarget))
                return;
            else if (ai->GetEnergyAmount() >= 60 && ai->CastSpell(AMBUSH, pTarget))
                return;
            else if (!ai->HasAura(CHEAP_SHOT, pTarget) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(CHEAP_SHOT, pTarget))
                return;
            else if (ai->GetEnergyAmount() >= 50 && ai->CastSpell(GARROTE, pTarget))
                return;
                
            return;
                
        case RogueSpeThreat:
            if (ai->GetEnergyAmount() >= 45 && !ai->HasAura(GOUGE, pTarget) && ai->CastSpell(GOUGE, pTarget))
                return;
            else if (ai->GetHealthPercent() <= 35 && !ai->HasAura(EVASION, m_bot) && ai->CastSpell(EVASION))
                return;
            else if (ai->GetHealthPercent() <= 30 && !ai->HasAura(BLIND, pTarget) && ai->GetEnergyAmount() >= 30 && ai->CastSpell(BLIND, pTarget))
                return;
            else if (ai->GetHealthPercent() <= 25 && ai->GetEnergyAmount() >= 20 && ai->CastSpell(FEINT))
                return;
            else if (ai->GetHealthPercent() <= 20 && !ai->HasAura(FEINT, m_bot) && ai->CastSpell(VANISH))
                return;
            else if (ai->CastSpell(PREPARATION))
                return;
            else if (m_bot->getRace() == RACE_NIGHTELF && ai->GetHealthPercent() <= 15 && !ai->HasAura(SHADOWMELD, m_bot) && ai->CastSpell(SHADOWMELD, m_bot))
                return;
                
            return;                
                
        case RogueSpellPreventing:
            if (ai->GetEnergyAmount() >= 25 && m_bot->GetComboPoints() >= 2 && ai->CastSpell(KIDNEY_SHOT, pTarget))
                return;
            else if (ai->GetEnergyAmount() >= 25  && ai->CastSpell(KICK, pTarget))
                return;
                
            return;                
                
        case RogueSpeCombat:
        default:
            if (m_bot->GetComboPoints() <= 4)
            {
                if (!ai->HasAura(SHADOW_DANCE, m_bot) && ai->CastSpell(SHADOW_DANCE, m_bot))
                    return;
                else if (ai->HasAura(SHADOW_DANCE, m_bot) && !ai->HasAura(CHEAP_SHOT, pTarget) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(CHEAP_SHOT, pTarget))
                    return;
                else if (ai->HasAura(SHADOW_DANCE, m_bot) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(AMBUSH, pTarget))
                    return;
                else if (ai->HasAura(SHADOW_DANCE, m_bot) && ai->GetEnergyAmount() >= 50 && ai->CastSpell(GARROTE, pTarget))
                    return;
                else if (pTarget->isInBackInMap(m_bot, 1) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(BACKSTAB, pTarget))
                    return;
                else if (ai->GetEnergyAmount() >= 60 && ai->CastSpell(MUTILATE, pTarget))
                    return;
                else if (ai->GetEnergyAmount() >= 45 && ai->CastSpell(SINISTER_STRIKE, pTarget))
                    return;
                else if (ai->GetEnergyAmount() >= 40 && ai->CastSpell(GHOSTLY_STRIKE, pTarget))
                    return;
                else if (ai->GetEnergyAmount() >= 35 && ai->CastSpell(HEMORRHAGE, pTarget))
                    return;
                else if (!pTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(DISMANTLE, pTarget))
                    return;
                else if (ai->GetEnergyAmount() >= 10 && ai->CastSpell(SHADOWSTEP, pTarget))
                    return;
                else if (m_bot->getRace() == RACE_BLOODELF && !ai->HasAura(ARCANE_TORRENT, pTarget) && ai->CastSpell(ARCANE_TORRENT, pTarget))
                    return;
                else if (m_bot->getRace() == RACE_HUMAN && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(EVERY_MAN_FOR_HIMSELF, m_bot))
                    return;
                else if (m_bot->getRace() == RACE_UNDEAD_PLAYER && m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(WILL_OF_THE_FORSAKEN, m_bot))
                    return;
                else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && ai->CastSpell(STONEFORM, m_bot))
                    return;
                else if (m_bot->getRace() == RACE_GNOME && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) && ai->CastSpell(ESCAPE_ARTIST, m_bot))
                    return;
                else if (m_bot->getRace() == RACE_ORC && !ai->HasAura(BLOOD_FURY, m_bot) && ai->CastSpell(BLOOD_FURY, m_bot))
                    return;
                else if (m_bot->getRace() == RACE_TROLL && !ai->HasAura(BERSERKING, m_bot) && ai->CastSpell(BERSERKING, m_bot))
                    return;
            }
            else
            {
                if (pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 25 && ai->CastSpell(KIDNEY_SHOT, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 25 && ai->CastSpell(SLICE_DICE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 25 && ai->CastSpell(SLICE_DICE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_WARRIOR && !ai->HasAura(EXPOSE_ARMOR, pTarget) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_PALADIN && !ai->HasAura(EXPOSE_ARMOR, pTarget) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_DEATH_KNIGHT && !ai->HasAura(EXPOSE_ARMOR, pTarget) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 25 && ai->CastSpell(RUPTURE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 25 && ai->CastSpell(RUPTURE, pTarget))
                    return;
                else if (ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, pTarget))
                    return;
            }
            
            return;            
    }
}

void PlayerbotRogueAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // remove stealth
    if (ai->HasAura(STEALTH, m_bot))
        m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();        
    
    // hp check
    if (pItemFood && ai->GetHealthPercent() < 30)
    {
        ai->UseItem(*pItemFood);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    else if (!pItemFood && pItemBandage && !ai->HasAura(RECENTLY_BANDAGED, m_bot) && ai->GetHealthPercent() < 70)
    {
        ai->UseItem(*pItemBandage);
        ai->SetIgnoreUpdateTime(8);
        return;
    }

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
            ai->UseItem(*poison, EQUIPMENT_SLOT_MAINHAND);
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
            ai->UseItem(*poison, EQUIPMENT_SLOT_OFFHAND);
            ai->SetIgnoreUpdateTime(5);
        }
    }

}
