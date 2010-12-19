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

#include "PlayerbotWarlockAI.h"

class PlayerbotAI;

PlayerbotWarlockAI::PlayerbotWarlockAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
    m_demonSummonFailed = false;
}

void PlayerbotWarlockAI::InitSpells(PlayerbotAI* const ai)
{
    // DESTRUCTION
    SHADOW_BOLT           = ai->initSpell(SHADOW_BOLT_1);
    IMMOLATE              = ai->initSpell(IMMOLATE_1);
    INCINERATE            = ai->initSpell(INCINERATE_1);
    SEARING_PAIN          = ai->initSpell(SEARING_PAIN_1);
    CONFLAGRATE           = ai->initSpell(CONFLAGRATE_1);
    SHADOWFURY            = ai->initSpell(SHADOWFURY_1);
    CHAOS_BOLT            = ai->initSpell(CHAOS_BOLT_1);
    SHADOWFLAME           = ai->initSpell(SHADOWFLAME_1);
    HELLFIRE              = ai->initSpell(HELLFIRE_1);
    RAIN_OF_FIRE          = ai->initSpell(RAIN_OF_FIRE_1);
    SOUL_FIRE             = ai->initSpell(SOUL_FIRE_1); // soul shard spells
    SHADOWBURN            = ai->initSpell(SHADOWBURN_1);
    // CURSE
    CURSE_OF_WEAKNESS     = ai->initSpell(CURSE_OF_WEAKNESS_1);
    CURSE_OF_THE_ELEMENTS = ai->initSpell(CURSE_OF_THE_ELEMENTS_1);
    CURSE_OF_AGONY        = ai->initSpell(CURSE_OF_AGONY_1);
    CURSE_OF_EXHAUSTION   = ai->initSpell(CURSE_OF_EXHAUSTION_1);
    CURSE_OF_TONGUES      = ai->initSpell(CURSE_OF_TONGUES_1);
    CURSE_OF_DOOM         = ai->initSpell(CURSE_OF_DOOM_1);
    // AFFLICTION
    CORRUPTION            = ai->initSpell(CORRUPTION_1);
    DRAIN_SOUL            = ai->initSpell(DRAIN_SOUL_1);
    DRAIN_LIFE            = ai->initSpell(DRAIN_LIFE_1);
    DRAIN_MANA            = ai->initSpell(DRAIN_MANA_1);
    LIFE_TAP              = ai->initSpell(LIFE_TAP_1);
    UNSTABLE_AFFLICTION   = ai->initSpell(UNSTABLE_AFFLICTION_1);
    HAUNT                 = ai->initSpell(HAUNT_1);
    SEED_OF_CORRUPTION    = ai->initSpell(SEED_OF_CORRUPTION_1);
    DARK_PACT             = ai->initSpell(DARK_PACT_1);
    HOWL_OF_TERROR        = ai->initSpell(HOWL_OF_TERROR_1);
    FEAR                  = ai->initSpell(FEAR_1);
    // DEMONOLOGY
    DEMON_SKIN            = ai->initSpell(DEMON_SKIN_1);
    DEMON_ARMOR           = ai->initSpell(DEMON_ARMOR_1);
    FEL_ARMOR             = ai->initSpell(FEL_ARMOR_1);
    SHADOW_WARD           = ai->initSpell(SHADOW_WARD_1);
    SOULSHATTER           = ai->initSpell(SOULSHATTER_1);
    SOUL_LINK             = ai->initSpell(SOUL_LINK_1);
    SOUL_LINK_AURA        = 25228; // dummy aura applied, after spell SOUL_LINK
    HEALTH_FUNNEL         = ai->initSpell(HEALTH_FUNNEL_1);
    DETECT_INVISIBILITY   = ai->initSpell(DETECT_INVISIBILITY_1);
    CREATE_FIRESTONE      = ai->initSpell(CREATE_FIRESTONE_1);
    // demon summon
    SUMMON_IMP            = ai->initSpell(SUMMON_IMP_1);
    SUMMON_VOIDWALKER     = ai->initSpell(SUMMON_VOIDWALKER_1);
    SUMMON_SUCCUBUS       = ai->initSpell(SUMMON_SUCCUBUS_1);
    SUMMON_FELHUNTER      = ai->initSpell(SUMMON_FELHUNTER_1);
    SUMMON_FELGUARD       = ai->initSpell(SUMMON_FELGUARD_1);
    // demon skills should be initialized on demons
    BLOOD_PACT            = 0; // imp skill
    CONSUME_SHADOWS       = 0; // voidwalker skill
    FEL_INTELLIGENCE      = 0; // felhunter skill

    RECENTLY_BANDAGED     = 11196; // first aid check

    // racial
    ARCANE_TORRENT        = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    ESCAPE_ARTIST         = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BLOOD_FURY            = ai->initSpell(BLOOD_FURY_WARLOCK); // orc
    WILL_OF_THE_FORSAKEN  = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotWarlockAI::~PlayerbotWarlockAI() {}

void PlayerbotWarlockAI::DoNextCombatManeuver(Unit* pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
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

    //ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, m_master ); // dont want to melee mob

    ai->SetInFront(pTarget);
    Unit* pVictim = pTarget->getVictim();
    Pet *pet = m_bot->GetPet();

    // Damage Spells
    ai->SetInFront(pTarget);

    switch (SpellSequence)
    {
        case SPELL_CURSES:
            if (!ai->HasAura(CURSE_OF_AGONY, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && LastSpellCurse < 1 && ai->GetManaPercent() >= 14 && ai->CastSpell(CURSE_OF_AGONY, pTarget))
            {
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
            }
            else if (!ai->HasAura(CURSE_OF_THE_ELEMENTS, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && !ai->HasAura(CURSE_OF_AGONY, pTarget) && !ai->HasAura(CURSE_OF_WEAKNESS, pTarget) && LastSpellCurse < 2 && ai->GetManaPercent() >= 10 && ai->CastSpell(CURSE_OF_THE_ELEMENTS, pTarget))
            {
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
            }
            else if (!ai->HasAura(CURSE_OF_EXHAUSTION, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && !ai->HasAura(CURSE_OF_WEAKNESS, pTarget) && !ai->HasAura(CURSE_OF_AGONY, pTarget) && !ai->HasAura(CURSE_OF_THE_ELEMENTS, pTarget) && LastSpellCurse < 3 && ai->GetManaPercent() >= 6 && ai->CastSpell(CURSE_OF_EXHAUSTION, pTarget))
            {
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
            }
            else if (!ai->HasAura(CURSE_OF_WEAKNESS, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && !ai->HasAura(CURSE_OF_EXHAUSTION, pTarget) && !ai->HasAura(CURSE_OF_AGONY, pTarget) && !ai->HasAura(CURSE_OF_THE_ELEMENTS, pTarget) && LastSpellCurse < 4 && ai->GetManaPercent() >= 14 && ai->CastSpell(CURSE_OF_WEAKNESS, pTarget))
            {
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
            }
            else if (!ai->HasAura(CURSE_OF_TONGUES, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && !ai->HasAura(CURSE_OF_WEAKNESS, pTarget) && !ai->HasAura(CURSE_OF_EXHAUSTION, pTarget) && !ai->HasAura(CURSE_OF_AGONY, pTarget) && !ai->HasAura(CURSE_OF_THE_ELEMENTS, pTarget) && LastSpellCurse < 5 && ai->GetManaPercent() >= 4 && ai->CastSpell(CURSE_OF_TONGUES, pTarget))
            {
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
            }
            else
                LastSpellCurse = 0;

            return;

        case SPELL_AFFLICTION:
            if (LastSpellAffliction < 1 && ai->GetManaPercent() <= 50 && ai->CastSpell(LIFE_TAP, m_bot))
            {
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (!ai->HasAura(CORRUPTION, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && !ai->HasAura(SEED_OF_CORRUPTION, pTarget) && LastSpellAffliction < 2 && ai->GetManaPercent() >= 19 && ai->CastSpell(CORRUPTION, pTarget))
            {
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.40 && !ai->HasAura(DRAIN_SOUL, pTarget) && LastSpellAffliction < 3 && ai->GetManaPercent() >= 19 && ai->CastSpell(DRAIN_SOUL, pTarget))
            {
                ai->SetIgnoreUpdateTime(15);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (LastSpellAffliction < 4 && !ai->HasAura(DRAIN_SOUL, pTarget) && !ai->HasAura(SEED_OF_CORRUPTION, pTarget) && !ai->HasAura(DRAIN_LIFE, pTarget) && !ai->HasAura(DRAIN_MANA, pTarget) && ai->GetHealthPercent() <= 70 && ai->GetManaPercent() >= 23 && ai->CastSpell(DRAIN_LIFE, pTarget))
            {
                ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (pTarget->GetPower(POWER_MANA) > 0 && !ai->HasAura(DRAIN_SOUL, pTarget) && !ai->HasAura(DRAIN_MANA, pTarget) && !ai->HasAura(SEED_OF_CORRUPTION, pTarget) && !ai->HasAura(DRAIN_LIFE, pTarget) && LastSpellAffliction < 5 && ai->GetManaPercent() < 70 && ai->GetManaPercent() >= 17 && ai->CastSpell(DRAIN_MANA, pTarget))
            {
                ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (LastSpellAffliction < 6 && !ai->HasAura(UNSTABLE_AFFLICTION, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && ai->GetManaPercent() >= 20 && ai->CastSpell(UNSTABLE_AFFLICTION, pTarget))
            {
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (LastSpellAffliction < 7 && !ai->HasAura(HAUNT, pTarget) && ai->GetManaPercent() >= 12 && ai->CastSpell(HAUNT, pTarget))
            {
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (!ai->HasAura(SEED_OF_CORRUPTION, pTarget) && LastSpellAffliction < 9 && ai->GetManaPercent() >= 34 && ai->CastSpell(SEED_OF_CORRUPTION, pTarget))
            {
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (!ai->HasAura(HOWL_OF_TERROR, pTarget) && ai->GetAttackerCount() > 3 && LastSpellAffliction < 10 && ai->GetManaPercent() >= 11 && ai->CastSpell(HOWL_OF_TERROR, pTarget))
            {
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (!ai->HasAura(FEAR, pTarget) && pVictim == m_bot && ai->GetAttackerCount() >= 2 && LastSpellAffliction < 11 && ai->GetManaPercent() >= 12 && ai->CastSpell(FEAR, pTarget))
            {
                ai->SetIgnoreUpdateTime(1.5);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else if (pet && ai->GetManaPercent() <= 50 && LastSpellAffliction < 12 && pet->GetPower(POWER_MANA) > 0 && ai->CastSpell(DARK_PACT, m_bot))
            {
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
            }
            else
                LastSpellAffliction = 0;
                
            return;

        case SPELL_DESTRUCTION:
            if (LastSpellDestruction < 1 && !ai->HasAura(SHADOWFURY, pTarget) && ai->GetManaPercent() >= 37 && ai->CastSpell(SHADOWFURY, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 2 && ai->GetManaPercent() >= 23 && ai->CastSpell(SHADOW_BOLT, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 3 && ai->GetAttackerCount() >= 3 && ai->GetManaPercent() >= 77 && ai->CastSpell(RAIN_OF_FIRE, pTarget))
            {
                ai->SetIgnoreUpdateTime(8);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (!ai->HasAura(SHADOWFLAME, pTarget) && LastSpellDestruction < 4 && ai->GetManaPercent() >= 25 && ai->CastSpell(SHADOWFLAME, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (!ai->HasAura(IMMOLATE, pTarget) && !ai->HasAura(SHADOWFLAME, pTarget) && LastSpellDestruction < 5 && ai->GetManaPercent() >= 23 && ai->CastSpell(IMMOLATE, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 6 && ai->GetManaPercent() >= 16 && ai->CastSpell(CONFLAGRATE, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 7 && ai->GetManaPercent() >= 19 && ai->CastSpell(INCINERATE, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 8 && ai->GetManaPercent() >= 11 && ai->CastSpell(SEARING_PAIN, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 9 && ai->GetManaPercent() >= 13 && ai->CastSpell(SOUL_FIRE, pTarget))
            {
                ai->SetIgnoreUpdateTime(6);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 10 && ai->GetManaPercent() >= 9 && ai->CastSpell(CHAOS_BOLT, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 11 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && !ai->HasAura(SHADOWBURN, pTarget) && ai->GetManaPercent() >= 27 && ai->CastSpell(SHADOWBURN, pTarget))
            {
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else if (LastSpellDestruction < 12 && !ai->HasAura(HELLFIRE, m_bot) && ai->GetAttackerCount() >= 5 && ai->GetHealthPercent() >= 10 && ai->GetManaPercent() >= 87 && ai->CastSpell(HELLFIRE))
            {
                ai->SetIgnoreUpdateTime(15);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
            }
            else
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction = 0;
            }
        
            return;
    }
}

void PlayerbotWarlockAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetMaster();
    if (!m_master)
        return;
        
    // Destroy extra soul shards
    uint8 shardCount = m_bot->GetItemCount(SOUL_SHARD, false, NULL);
    uint8 freeSpace = ai->GetFreeBagSpace();
    if (shardCount > MAX_SHARD_COUNT || (freeSpace == 0 && shardCount > 1))
        m_bot->DestroyItemCount(SOUL_SHARD, shardCount > MAX_SHARD_COUNT ? shardCount - MAX_SHARD_COUNT : 1, true, false);

    SpellSequence = SPELL_CURSES;

    Pet *pet = m_bot->GetPet();

    // buff myself DEMON_SKIN, DEMON_ARMOR, FEL_ARMOR
    if (!ai->HasAura(FEL_ARMOR, m_bot) && ai->CastSpell(FEL_ARMOR, m_bot))
        return;
    else if (!ai->HasAura(DEMON_ARMOR, m_bot) && !ai->HasAura(FEL_ARMOR, m_bot) && ai->CastSpell(DEMON_ARMOR, m_bot))
        return;    
    else if (!ai->HasAura(DEMON_SKIN, m_bot) && !ai->HasAura(FEL_ARMOR, m_bot) && !ai->HasAura(DEMON_ARMOR, m_bot) && ai->CastSpell(DEMON_SKIN, m_bot))
        return;    

    // buff myself & master DETECT_INVISIBILITY
    else if (!ai->HasAura(DETECT_INVISIBILITY, m_bot) && ai->GetManaPercent() >= 2 && ai->CastSpell(DETECT_INVISIBILITY, m_bot))
        return;    
    else if (!ai->HasAura(DETECT_INVISIBILITY, m_master) && ai->GetManaPercent() >= 2 && ai->CastSpell(DETECT_INVISIBILITY, m_master))
        return;    

    // firestone creation and use - proof of concept for updated UseItem method.
    Item* const weapon = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        Item* const stone = ai->FindConsumable(FIRESTONE_DISPLAYID);
        if (!stone)
        {
            if (m_bot->HasItemCount(SOUL_SHARD, 1, false) && ai->CastSpell(CREATE_FIRESTONE))
                ai->SetIgnoreUpdateTime(5);
        }
        else
        {
            ai->UseItem(stone, EQUIPMENT_SLOT_MAINHAND);
            ai->SetIgnoreUpdateTime(5);
        }
    }

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemDrink = ai->FindDrink();
    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();        
    
    // mana check
    if (pItemDrink && ai->GetManaPercent() < 25)
    {
        ai->UseItem(pItemDrink);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    else if (pet && !pItemDrink && ai->GetManaPercent() <= 50 && pet->GetPower(POWER_MANA) > 0 && ai->CastSpell(DARK_PACT, m_bot))
        return;
    else if (!pet && !pItemDrink && ai->GetManaPercent() <= 50 && ai->GetHealthPercent() > 25 && ai->CastSpell(LIFE_TAP, m_bot))
        return;
    // hp check
    else if (pItemFood && ai->GetHealthPercent() < 30)
    {
        ai->UseItem(pItemFood);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    else if (!pItemFood && pItemBandage && !ai->HasAura(RECENTLY_BANDAGED, m_bot) && ai->GetHealthPercent() < 70)
    {
        ai->UseItem(pItemBandage);
        ai->SetIgnoreUpdateTime(8);
        return;
    }
    else if (pet && !pItemFood && !pItemBandage && !ai->HasAura(CONSUME_SHADOWS, m_bot) && ai->GetHealthPercent() < 75 && ai->CastSpell(CONSUME_SHADOWS, m_bot))
        return;

    // check for demon
    if (!m_demonSummonFailed && !pet)
    {
        // summon demon
        if (ai->CastSpell(SUMMON_FELGUARD, m_bot))
            return;
        else if (ai->CastSpell(SUMMON_FELHUNTER, m_bot))
            return;
        else if (ai->CastSpell(SUMMON_SUCCUBUS, m_bot))
            return;
        else if (ai->CastSpell(SUMMON_VOIDWALKER, m_bot))
            return;
        else if (ai->GetManaPercent() >= 64 && ai->CastSpell(SUMMON_IMP, m_bot))
            return;
        else
            m_demonSummonFailed = true;
    }

    // check for buffs with demon
    if (pet && !ai->HasAura(SOUL_LINK_AURA, m_bot) && ai->GetBaseManaPercent() >= 16 && ai->CastSpell(SOUL_LINK, m_bot))
        return;
    else if (pet && !ai->HasAura(BLOOD_PACT, m_bot) && ai->CastSpell(BLOOD_PACT, m_bot))
        return;
    else if (pet && !ai->HasAura(FEL_INTELLIGENCE, m_bot) && ai->CastSpell(FEL_INTELLIGENCE, m_bot))
        return;
}
