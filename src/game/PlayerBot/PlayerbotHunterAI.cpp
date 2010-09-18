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

#include "PlayerbotHunterAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotHunterAI::PlayerbotHunterAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
    m_petSummonFailed = false;
    m_rangedCombat = true;
}

void PlayerbotHunterAI::InitSpells(PlayerbotAI* const ai)
{
    // PET CTRL
    PET_SUMMON                    = ai->initSpell(CALL_PET_1);
    PET_DISMISS                   = ai->initSpell(DISMISS_PET_1);
    PET_REVIVE                    = ai->initSpell(REVIVE_PET_1);
    PET_MEND                      = ai->initSpell(MEND_PET_1);
    PET_FEED                      = 1539;

    INTIMIDATION                  = ai->initSpell(INTIMIDATION_1); // (generic)

    // PET SKILLS must be initialized by pets
    SONIC_BLAST                   = 0; // bat
    DEMORALIZING_SCREECH          = 0;
    BAD_ATTITUDE                  = 0; // crocolisk
    NETHER_SHOCK                  = 0;

    // RANGED COMBAT
    AUTO_SHOT                     = ai->initSpell(AUTO_SHOT_1);
    HUNTERS_MARK                  = ai->initSpell(HUNTERS_MARK_1);
    ARCANE_SHOT                   = ai->initSpell(ARCANE_SHOT_1);
    CONCUSSIVE_SHOT               = ai->initSpell(CONCUSSIVE_SHOT_1);
    DISTRACTING_SHOT              = ai->initSpell(DISTRACTING_SHOT_1);
    MULTI_SHOT                    = ai->initSpell(MULTISHOT_1);
    EXPLOSIVE_SHOT                = ai->initSpell(EXPLOSIVE_SHOT_1);
    SERPENT_STING                 = ai->initSpell(SERPENT_STING_1);
    SCORPID_STING                 = ai->initSpell(SCORPID_STING_1);
    WYVERN_STING                  = ai->initSpell(WYVERN_STING_1);
    VIPER_STING                   = ai->initSpell(VIPER_STING_1);
    AIMED_SHOT                    = ai->initSpell(AIMED_SHOT_1);
    STEADY_SHOT                   = ai->initSpell(STEADY_SHOT_1);
    CHIMERA_SHOT                  = ai->initSpell(CHIMERA_SHOT_1);
    VOLLEY                        = ai->initSpell(VOLLEY_1);
    BLACK_ARROW                   = ai->initSpell(BLACK_ARROW_1);
    KILL_SHOT                     = ai->initSpell(KILL_SHOT_1);

    // MELEE
    RAPTOR_STRIKE                 = ai->initSpell(RAPTOR_STRIKE_1);
    WING_CLIP                     = ai->initSpell(WING_CLIP_1);
    MONGOOSE_BITE                 = ai->initSpell(MONGOOSE_BITE_1);
    DISENGAGE                     = ai->initSpell(DISENGAGE_1);
    MISDIRECTION                  = ai->initSpell(MISDIRECTION_1);
    DETERRENCE                    = ai->initSpell(DETERRENCE_1);

    // TRAPS
    BEAR_TRAP                     = 0; // non-player spell
    FREEZING_TRAP                 = ai->initSpell(FREEZING_TRAP_1);
    IMMOLATION_TRAP               = ai->initSpell(IMMOLATION_TRAP_1);
    FROST_TRAP                    = ai->initSpell(FROST_TRAP_1);
    EXPLOSIVE_TRAP                = ai->initSpell(EXPLOSIVE_TRAP_1);
    ARCANE_TRAP                   = 0; // non-player spell
    SNAKE_TRAP                    = ai->initSpell(SNAKE_TRAP_1);

    // BUFFS
    ASPECT_OF_THE_HAWK            = ai->initSpell(ASPECT_OF_THE_HAWK_1);
    ASPECT_OF_THE_MONKEY          = ai->initSpell(ASPECT_OF_THE_MONKEY_1);
    RAPID_FIRE                    = ai->initSpell(RAPID_FIRE_1);
    TRUESHOT_AURA                 = ai->initSpell(TRUESHOT_AURA_1);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_HUNTER); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    BLOOD_FURY                    = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP                     = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING                    = ai->initSpell(BERSERKING_ALL); // troll

    m_petSummonFailed = false;
    m_rangedCombat = true;
}

PlayerbotHunterAI::~PlayerbotHunterAI() {}

bool PlayerbotHunterAI::HasPet(Player* bot)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM character_pet WHERE owner = '%u' AND (slot = '%u' OR slot = '%u')", bot->GetGUIDLow(), PET_SAVE_AS_CURRENT, PET_SAVE_NOT_IN_SLOT);

    if (result)
    {
        delete result;
        return true;  //hunter has current pet
    }
    else
        return false;  //hunter either has no pet or stabled
}

void PlayerbotHunterAI::DoNextCombatManeuver(Unit* pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    ai->SetInFront(pTarget);
    Unit* pVictim = pTarget->getVictim();
    Pet *pet = m_bot->GetPet();
    float dist = m_bot->GetDistance(pTarget);
    
    /*switch (ai->GetScenarioType())
    {
        //SCENARIO_PVEEASY,
        //SCENARIO_PVEHARD,
        //SCENARIO_DUEL,
        //SCENARIO_PVPEASY,
        //SCENARIO_PVPHARD
        //return;
    }*/
    
    // check for pet and heal if neccessary
    if (pet && (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f) && (!pet->getDeathState() != ALIVE && pVictim != m_bot && !ai->HasAura(PET_MEND, pet) && ai->GetManaPercent() >= 13 && ai->CastSpell(PET_MEND, m_bot)))
        return;
    else if (pet && pVictim == pet && !ai->HasAura(INTIMIDATION, pet) && ai->CastSpell(INTIMIDATION, m_bot))
        return;
        
    // racial traits
    if (m_bot->getRace() == RACE_ORC && !ai->HasAura(BLOOD_FURY, m_bot) && ai->CastSpell(BLOOD_FURY, m_bot))
        return;
    else if (m_bot->getRace() == RACE_TROLL && !ai->HasAura(BERSERKING, m_bot) && ai->CastSpell(BERSERKING, m_bot))
        return;
        
    // check if ranged combat is possible (set m_rangedCombat and switch auras
    if ((dist <= ATTACK_DISTANCE || !m_bot->GetUInt32Value(PLAYER_AMMO_ID)) && m_rangedCombat)
    {
        // switch to melee combat (target in melee range, out of ammo)
        m_rangedCombat = false;
        
        /*if (!m_bot->GetUInt32Value(PLAYER_AMMO_ID))
            ai->TellMaster("Out of ammo!");*/
            
        // become monkey (increases dodge chance)...
        if (!ai->HasAura(ASPECT_OF_THE_MONKEY, m_bot) && ai->CastSpell(ASPECT_OF_THE_MONKEY, m_bot))
            return;
    }
    else if (dist > ATTACK_DISTANCE && !m_rangedCombat)
    {
        // switch to ranged combat
        m_rangedCombat = true;
        
        // increase ranged attack power...
        if (!ai->HasAura(ASPECT_OF_THE_HAWK, m_bot) && ai->CastSpell(ASPECT_OF_THE_HAWK, m_bot))
            return;
    }
    
    // check if we have hawk aspect in ranged combat    
    if (m_rangedCombat && !ai->HasAura(ASPECT_OF_THE_HAWK, m_bot) && ai->CastSpell(ASPECT_OF_THE_HAWK, m_bot))
        return;
    // check if we have monkey aspect in melee combat        
    else if (!m_rangedCombat && !ai->HasAura(ASPECT_OF_THE_MONKEY, m_bot) && ai->CastSpell(ASPECT_OF_THE_MONKEY, m_bot))
        return;
    // activate auto shot
    else if (m_rangedCombat && !m_bot->FindCurrentSpellBySpellId(AUTO_SHOT) && ai->CastSpell(AUTO_SHOT, pTarget))
        return;
    else if (m_bot->FindCurrentSpellBySpellId(AUTO_SHOT))
    {
        m_bot->InterruptNonMeleeSpells(true, AUTO_SHOT);
        return;
    }
    // damage spells
    else if (m_rangedCombat)
    {
        if (ai->GetManaPercent() >= 3 && !ai->HasAura(HUNTERS_MARK, pTarget) && ai->CastSpell(HUNTERS_MARK, pTarget))
            return;
        else if (ai->GetManaPercent() >= 3 && !ai->HasAura(RAPID_FIRE, m_bot) && ai->CastSpell(RAPID_FIRE, m_bot))
            return;
        else if (ai->GetManaPercent() >= 13 && ai->GetAttackerCount() >= 3 && ai->CastSpell(MULTI_SHOT, pTarget))
            return;
        else if (ai->GetManaPercent() >= 7 && ai->CastSpell(ARCANE_SHOT, pTarget))
            return;
        else if (ai->GetManaPercent() >= 6 && !ai->HasAura(CONCUSSIVE_SHOT, pTarget) && ai->CastSpell(CONCUSSIVE_SHOT, pTarget))
            return;
        else if (ai->GetManaPercent() >= 10 && !ai->HasAura(EXPLOSIVE_SHOT, pTarget) && ai->CastSpell(EXPLOSIVE_SHOT, pTarget))
            return;
        else if (ai->GetManaPercent() >= 8 && pTarget->GetPower(POWER_MANA) > 0 && ai->GetManaPercent() < 70 && !ai->HasAura(VIPER_STING, pTarget) && ai->CastSpell(VIPER_STING, pTarget))
            return;
        else if (ai->GetManaPercent() >= 13 && !ai->HasAura(SERPENT_STING, pTarget) && !ai->HasAura(SCORPID_STING, pTarget) &&  !ai->HasAura(VIPER_STING, pTarget) && ai->CastSpell(SERPENT_STING, pTarget))
            return;
        else if (ai->GetManaPercent() >= 11 && !ai->HasAura(WYVERN_STING, pTarget) && !ai->HasAura(SCORPID_STING, pTarget) && !ai->HasAura(SERPENT_STING, pTarget) && !ai->HasAura(VIPER_STING, pTarget) && ai->CastSpell(SCORPID_STING, pTarget))
            return;
        else if (ai->GetManaPercent() >= 12 && ai->CastSpell(CHIMERA_SHOT, pTarget))
            return;
        else if (ai->GetManaPercent() >= 24 && ai->GetAttackerCount() >= 3 && ai->CastSpell(VOLLEY, pTarget))
            return;
        else if (ai->GetManaPercent() >= 6 && !ai->HasAura(BLACK_ARROW, pTarget) && ai->CastSpell(BLACK_ARROW, pTarget))
            return;
        else if (ai->GetManaPercent() >= 12 && ai->CastSpell(AIMED_SHOT, pTarget))
            return;
        else if (ai->GetManaPercent() >= 5 && ai->CastSpell(STEADY_SHOT, pTarget))
            return;
        else if (ai->GetManaPercent() >= 7 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.2 && ai->CastSpell(KILL_SHOT, pTarget))
            return;
    }
    else
    {
        if (ai->GetManaPercent() >= 6 && ai->CastSpell(RAPTOR_STRIKE, pTarget))
            return;
        else if (ai->GetManaPercent() >= 27 && !ai->HasAura(EXPLOSIVE_TRAP, pTarget) && !ai->HasAura(ARCANE_TRAP, pTarget) && !ai->HasAura(IMMOLATION_TRAP, pTarget) && !ai->HasAura(FROST_TRAP, pTarget) && !ai->HasAura(BEAR_TRAP, pTarget) && ai->CastSpell(EXPLOSIVE_TRAP, pTarget))
            return;
        else if (ai->GetManaPercent() >= 6 && !ai->HasAura(WING_CLIP, pTarget) && ai->CastSpell(WING_CLIP, pTarget))
            return;
        else if (ai->GetManaPercent() >= 13 && !ai->HasAura(IMMOLATION_TRAP, pTarget) && !ai->HasAura(ARCANE_TRAP, pTarget) && !ai->HasAura(EXPLOSIVE_TRAP, pTarget) && !ai->HasAura(FROST_TRAP, pTarget) && !ai->HasAura(BEAR_TRAP, pTarget) && ai->CastSpell(IMMOLATION_TRAP, pTarget))
            return;
        else if (ai->GetManaPercent() >= 4 && ai->CastSpell(MONGOOSE_BITE, pTarget))
            return;
        else if (ai->GetManaPercent() >= 2 && !ai->HasAura(FROST_TRAP, pTarget) && !ai->HasAura(ARCANE_TRAP, pTarget) && !ai->HasAura(IMMOLATION_TRAP, pTarget) && !ai->HasAura(EXPLOSIVE_TRAP, pTarget) && !ai->HasAura(BEAR_TRAP, pTarget) && ai->CastSpell(FROST_TRAP, pTarget))
            return;
        else if (!ai->HasAura(ARCANE_TRAP, pTarget) && !ai->HasAura(BEAR_TRAP, pTarget) && !ai->HasAura(EXPLOSIVE_TRAP, pTarget) && !ai->HasAura(IMMOLATION_TRAP, pTarget) && !ai->HasAura(FROST_TRAP, pTarget) && ai->CastSpell(ARCANE_TRAP, pTarget))
            return;
        else if (pVictim == m_bot && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && !ai->HasAura(DETERRENCE, m_bot) && ai->CastSpell(DETERRENCE, m_bot))
            return;
        else if (m_bot->getRace() == RACE_TAUREN && !ai->HasAura(WAR_STOMP, pTarget) && ai->CastSpell(WAR_STOMP, pTarget))
            return;
        else if (m_bot->getRace() == RACE_BLOODELF && !ai->HasAura(ARCANE_TORRENT, pTarget) && ai->CastSpell(ARCANE_TORRENT, pTarget))
            return;
        else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && ai->CastSpell(STONEFORM, m_bot))
            return;
        else if (m_bot->getRace() == RACE_NIGHTELF && pVictim == m_bot && ai->GetHealthPercent() < 25 && !ai->HasAura(SHADOWMELD, m_bot) && ai->CastSpell(SHADOWMELD, m_bot))
            return;
        else if (m_bot->getRace() == RACE_DRAENEI && ai->GetHealthPercent() < 25 && !ai->HasAura(GIFT_OF_THE_NAARU, m_bot) && ai->CastSpell(GIFT_OF_THE_NAARU, m_bot))
            return;
        else if (pet && !pet->getDeathState() != ALIVE && pVictim == m_bot && !ai->HasAura(MISDIRECTION, m_bot) && ai->GetManaPercent() >= 9 && ai->CastSpell(MISDIRECTION, pet))
            return;
        else if (FREEZING_TRAP>0 && ai->GetManaPercent()>=5 && !ai->HasAura(FREEZING_TRAP, pTarget) && !ai->HasAura(ARCANE_TRAP, pTarget) && !ai->HasAura(EXPLOSIVE_TRAP, pTarget) && !ai->HasAura(BEAR_TRAP, pTarget) && !ai->HasAura(IMMOLATION_TRAP, pTarget) && !ai->HasAura(FROST_TRAP, pTarget) && ai->CastSpell(FREEZING_TRAP,pTarget))
            return;
        else if (BEAR_TRAP>0 && !ai->HasAura(BEAR_TRAP, pTarget) && !ai->HasAura(ARCANE_TRAP, pTarget) && !ai->HasAura(EXPLOSIVE_TRAP, pTarget) && !ai->HasAura(IMMOLATION_TRAP, pTarget) && !ai->HasAura(FROST_TRAP, pTarget) && ai->CastSpell(BEAR_TRAP,pTarget))
            return;
        else if (DISENGAGE>0 && pVictim && ai->GetManaPercent()>=5 && ai->CastSpell(DISENGAGE,pTarget))
            return;
    }
}

void PlayerbotHunterAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return;
        
    // reset ranged combat state
    if (!m_rangedCombat)
        m_rangedCombat = true;

    // buff group
    if (!ai->HasAura(TRUESHOT_AURA, m_bot) && ai->CastSpell(TRUESHOT_AURA, m_bot))
        return;

    // buff myself
    else if (!ai->HasAura(ASPECT_OF_THE_HAWK, m_bot) && ai->CastSpell(ASPECT_OF_THE_HAWK, m_bot))
        return;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemDrink = ai->FindDrink();
    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();        
    
    // mana check        
    if (pItemDrink && ai->GetManaPercent() < 30)
    {
        ai->UseItem(*pItemDrink);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    // hp check
    else if (pItemFood && ai->GetHealthPercent() < 30)
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
    else if (!pItemFood && !pItemBandage && m_bot->getRace() == RACE_DRAENEI && !ai->HasAura(GIFT_OF_THE_NAARU, m_bot) && ai->GetHealthPercent() < 70 && ai->CastSpell(GIFT_OF_THE_NAARU, m_bot))
        return;

    // check for pet
    if (!m_petSummonFailed && HasPet(m_bot))
    {
        // we can summon pet, and no critical summon errors before
        Pet *pet = m_bot->GetPet();
        if (!pet)
        {
            // summon pet
            if (!ai->CastSpell(PET_SUMMON, m_bot))
                m_petSummonFailed = true;
            return;
        }
        else if (pet->getDeathState() != ALIVE)
        {
            // revive pet
            if (ai->GetManaPercent() >= 80 && ai->CastSpell(PET_REVIVE, m_bot))
                return;
        }
        else if (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f)
        {
            // heal pet when health lower 50%
            if (!pet->getDeathState() != ALIVE && !ai->HasAura(PET_MEND, pet) && ai->GetManaPercent() >= 13 && ai->CastSpell(PET_MEND, m_bot))
                return;
        }
        else if (pet->GetHappinessState() != HAPPY) // if pet is hungry
        {
            Unit *caster = (Unit*) m_bot;
            // list out items in main backpack
            for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
            {
                Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                if (!pItem)
                    continue;
                    
                const ItemPrototype* const pItemProto = pItem->GetProto();
                if (!pItemProto)
                    continue;

                if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                {
                    //sLog.outDebug("Food for pet: %s",pItemProto->Name1);
                    caster->CastSpell(caster, 51284, true); // pet feed visual
                    uint32 count = 1; // number of items used
                    int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                    m_bot->DestroyItemCount(pItem, count, true); // remove item from inventory
                    m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true); // feed pet
                    ai->SetIgnoreUpdateTime(10);
                    return;
                }
            }
            // list out items in other removable backpacks
            for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
            {
                const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                if (!pBag)
                    continue;
                    
                for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                {
                    Item* const pItem = m_bot->GetItemByPos(bag, slot);
                    if (!pItem)
                        continue;
                        
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                    {
                        //sLog.outDebug("Food for pet: %s",pItemProto->Name1);
                        caster->CastSpell(caster, 51284, true); // pet feed visual
                        uint32 count = 1; // number of items used
                        int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                        m_bot->DestroyItemCount(pItem, count, true); // remove item from inventory
                        m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true); // feed pet
                        ai->SetIgnoreUpdateTime(10);
                        return;
                    }
                }
            }
            if (ai->HasAura(PET_MEND, pet) && !ai->HasAura(PET_FEED, pet))
                return;
            ai->SetIgnoreUpdateTime(7);
        }
    }
}
