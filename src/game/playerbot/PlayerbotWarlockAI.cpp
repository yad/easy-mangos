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
    LastSpellCurses = 0;
    LastSpellSummoning = 0;
    LastSpellDestruction = 0;
    petInitAi = false;
}

bool PlayerbotWarlockAI::InitPet()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Pet* demon = m_bot->GetPet();

    if (m_bot->getRole()==WarlockSummoning)
    {
        if (!demon)
        {
            if (ai->CastSpell(SUMMON_FELGUARD, m_bot))
            {
                petInitAi = true;
                return true;
            }
            else if (ai->CastSpell(SUMMON_FELHUNTER, m_bot))
            {
                petInitAi = true;
                return true;
            }
            else if (ai->CastSpell(SUMMON_SUCCUBUS, m_bot))
            {
                petInitAi = true;
                return true;
            }
            else if (ai->CastSpell(SUMMON_VOIDWALKER, m_bot))
            {
                petInitAi = true;
                return true;
            }
            else if (ai->CastSpell(SUMMON_IMP, m_bot))
            {
                petInitAi = true;
                return true;
            }
        }
        else
        {
            if (IsBetterPet(DEMON_FELGUARD) && m_bot->HasSpell(SUMMON_FELGUARD))
            {
                m_bot->RemovePet(PET_SAVE_AS_DELETED);
                if (ai->CastSpell(SUMMON_FELGUARD, m_bot))
                {
                    petInitAi = true;
                    return true;
                }
            }
            else if (IsBetterPet(DEMON_FELHUNTER) && m_bot->HasSpell(SUMMON_FELHUNTER))
            {
                m_bot->RemovePet(PET_SAVE_AS_DELETED);
                if (ai->CastSpell(SUMMON_FELHUNTER, m_bot))
                {
                    petInitAi = true;
                    return true;
                }
            }
            else if (IsBetterPet(DEMON_SUCCUBUS) && m_bot->HasSpell(SUMMON_SUCCUBUS))
            {
                m_bot->RemovePet(PET_SAVE_AS_DELETED);
                if (ai->CastSpell(SUMMON_SUCCUBUS, m_bot))
                {
                    petInitAi = true;
                    return true;
                }
            }
            else if (IsBetterPet(DEMON_VOIDWALKER) && m_bot->HasSpell(SUMMON_VOIDWALKER))
            {
                m_bot->RemovePet(PET_SAVE_AS_DELETED);
                if (ai->CastSpell(SUMMON_VOIDWALKER, m_bot))
                {
                    petInitAi = true;
                    return true;
                }
            }
            else if (IsBetterPet(DEMON_IMP) && m_bot->HasSpell(SUMMON_IMP))
            {
                m_bot->RemovePet(PET_SAVE_AS_DELETED);
                if (ai->CastSpell(SUMMON_IMP, m_bot))
                {
                    petInitAi = true;
                    return true;
                }
            }
        }
    }
    else
    {
        if (demon && demon->GetEntry()==DEMON_IMP)
            return false;

        if (ai->CastSpell(SUMMON_IMP, m_bot))
        {
            petInitAi = true;
            return true;
        }
    }
    return false;
}

bool PlayerbotWarlockAI::IsBetterPet(DemonEntry newdemon)
{
    DemonEntry currentdemon = DemonEntry(GetPlayerBot()->GetPet()->GetEntry());

    if (newdemon==currentdemon)
        return false;

    switch (newdemon)
    {
        case DEMON_IMP:
        {
            return false;
        }
        case DEMON_VOIDWALKER:
        {
            if (currentdemon==DEMON_IMP)
                return true;
            return false;
        }
        case DEMON_SUCCUBUS:
        {
            if (currentdemon==DEMON_IMP||currentdemon==DEMON_VOIDWALKER)
                return true;
            return false;
        }
        case DEMON_FELHUNTER:
        {
            if (currentdemon==DEMON_IMP||currentdemon==DEMON_VOIDWALKER
                ||currentdemon==DEMON_SUCCUBUS)
                return true;
            return false;
        }
        case DEMON_FELGUARD:
        {
            if (currentdemon==DEMON_IMP||currentdemon==DEMON_VOIDWALKER
                ||currentdemon==DEMON_SUCCUBUS||currentdemon==DEMON_FELHUNTER)
                return true;
            return false;
        }
        default:
            return false;
    }
}

void PlayerbotWarlockAI::ReinitCycles()
{
    LastSpellCurses = 0;
    LastSpellSummoning = 0;
    LastSpellDestruction = 0;
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
    SIPHON_LIFE           = ai->initSpell(SIPHON_LIFE_1);
    // DEMONOLOGY
    DEMON_SKIN            = ai->initSpell(DEMON_SKIN_1);
    DEMON_ARMOR           = ai->initSpell(DEMON_ARMOR_1);
    DEMONIC_EMPOWERMENT   = ai->initSpell(DEMONIC_EMPOWERMENT_1);
    FEL_ARMOR             = ai->initSpell(FEL_ARMOR_1);
    SHADOW_WARD           = ai->initSpell(SHADOW_WARD_1);
    SOULSHATTER           = ai->initSpell(SOULSHATTER_1);
    SOUL_LINK             = ai->initSpell(SOUL_LINK_1);
    SOUL_LINK_AURA        = 25228; // dummy aura applied, after spell SOUL_LINK
    HEALTH_FUNNEL         = ai->initSpell(HEALTH_FUNNEL_1);
    DETECT_INVISIBILITY   = ai->initSpell(DETECT_INVISIBILITY_1);
    CREATE_FIRESTONE      = ai->initSpell(CREATE_FIRESTONE_1);
    CREATE_HEALTHSTONE    = ai->initSpell(CREATE_HEALTHSTONE_1);
    CREATE_SOULSTONE      = ai->initSpell(CREATE_SOULSTONE_1);
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
    InitPet();
}

PlayerbotWarlockAI::~PlayerbotWarlockAI() {}

void PlayerbotWarlockAI::DoCombatManeuver(Unit *pTarget, bool cac)
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    switch (m_bot->getRole())
    {
        case WarlockDestruction:
        {
            static const uint32 SpellFire[] = {CURSE_OF_AGONY, IMMOLATE, INCINERATE};
            static uint32 elt = sizeof(SpellFire)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                uint32 spellId = SpellFire[(i+LastSpellDestruction)%elt];
                if (spellId==INCINERATE && !m_bot->HasSpell(INCINERATE))
                   spellId = SHADOW_BOLT;
                if (ai->CastSpell(spellId, pTarget))
                {
                    LastSpellDestruction = (i+LastSpellDestruction)%elt;
                    break;
                }
            }
            break;
        }
        case WarlockCurses:
        {
            static const uint32 SpellArcane[] = {HAUNT, SHADOW_BOLT, UNSTABLE_AFFLICTION, IMMOLATE,
                CORRUPTION, CURSE_OF_AGONY, SIPHON_LIFE};
            static uint32 elt = sizeof(SpellArcane)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                uint32 spellId = SpellArcane[(i+LastSpellCurses)%elt];

                if (spellId==IMMOLATE && pTarget->GetHealthPercent() < 35.0f)
                   continue;
                if (spellId==SHADOW_BOLT && pTarget->GetHealthPercent() < 25.0f)
                   spellId=DRAIN_SOUL;

                if (ai->CastSpell(spellId, pTarget))
                {
                    LastSpellCurses = (i+LastSpellCurses)%elt;
                    break;
                }
            }
            break;
        }
        case WarlockSummoning:
        {
            static const uint32 SpellFrost[] = {CURSE_OF_AGONY, CORRUPTION, IMMOLATE, INCINERATE};
            static uint32 elt = sizeof(SpellFrost)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                uint32 spellId = SpellFrost[(i+LastSpellSummoning)%elt];
                if (spellId==INCINERATE && !m_bot->HasSpell(INCINERATE))
                   spellId = SHADOW_BOLT;
                if (ai->CastSpell(spellId, pTarget))
                {
                    LastSpellSummoning = (i+LastSpellSummoning)%elt;
                    break;
                }
            }
            break;
        }
    }
}

void PlayerbotWarlockAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    if (!ai->CastAura(FEL_ARMOR, m_bot))
        if (!ai->CastAura(DEMON_ARMOR, m_bot))
            if (ai->CastAura(DEMON_SKIN, m_bot))
                return;

    uint8 shardCount = m_bot->GetItemCount(SOUL_SHARD, false, NULL);
    uint8 freeSpace = ai->GetFreeBagSpace();
    if (shardCount > MAX_SHARD_COUNT && freeSpace == 0 && shardCount > 1)
        m_bot->DestroyItemCount(SOUL_SHARD, shardCount > MAX_SHARD_COUNT ? shardCount - MAX_SHARD_COUNT : 1, true, false);
    else if(shardCount==0 && freeSpace!=0)
        m_bot->StoreNewItemInBestSlots(SOUL_SHARD, 1);

    if (InitPet())
        return;

    Pet *pet = m_bot->GetPet();
    if (pet)
    {
        if (petInitAi)
        {
            switch (pet->GetEntry())
            {
                case DEMON_IMP:
                {
                    BLOOD_PACT       = ai->initPetSpell(BLOOD_PACT_ICON);
                    FIREBOLT         = ai->initPetSpell(FIREBOLT_ICON);
                    FIRE_SHIELD      = ai->initPetSpell(FIRE_SHIELD_ICON);
                    break;
                }
                case DEMON_VOIDWALKER:
                {
                    CONSUME_SHADOWS  = ai->initPetSpell(CONSUME_SHADOWS_ICON);
                    SACRIFICE        = ai->initPetSpell(SACRIFICE_ICON);
                    SUFFERING        = ai->initPetSpell(SUFFERING_ICON);
                    TORMENT          = ai->initPetSpell(TORMENT_ICON);
                    break;
                }
                case DEMON_SUCCUBUS:
                {
                    LASH_OF_PAIN     = ai->initPetSpell(LASH_OF_PAIN_ICON);
                    SEDUCTION        = ai->initPetSpell(SEDUCTION_ICON);
                    SOOTHING_KISS    = ai->initPetSpell(SOOTHING_KISS_ICON);
                    break;
                }
                case DEMON_FELHUNTER:
                {
                    DEVOUR_MAGIC     = ai->initPetSpell(DEVOUR_MAGIC_ICON);
                    FEL_INTELLIGENCE = ai->initPetSpell(FEL_INTELLIGENCE_ICON);
                    SHADOW_BITE      = ai->initPetSpell(SHADOW_BITE_ICON);
                    SPELL_LOCK       = ai->initPetSpell(SPELL_LOCK_ICON);
                    break;
                }
                case DEMON_FELGUARD:
                {
                    ANGUISH          = ai->initPetSpell(ANGUISH_ICON);
                    CLEAVE           = ai->initPetSpell(CLEAVE_ICON);
                    INTERCEPT        = ai->initPetSpell(INTERCEPT_ICON);
                    break;
                }
            }
            petInitAi = false;
        }

        if (!m_bot->HasAura(SOUL_LINK_AURA) && ai->CastSpell(SOUL_LINK, m_bot))
            return;

        switch(pet->GetEntry())
        {
            case DEMON_IMP:
            {
                if (pet->GetHealthPercent() < 75 && ai->CastPetAura(BLOOD_PACT, m_bot))
                    return;
                break;
            }
            case DEMON_VOIDWALKER:
            {
                if (pet->GetHealthPercent() < 75 && ai->CastPetAura(CONSUME_SHADOWS, m_bot))
                    return;
                break;
            }
            case DEMON_FELHUNTER:
            {
                if (ai->CastPetAura(FEL_INTELLIGENCE, m_bot))
                    return;
                break;
            }
        }
    }

    if (m_bot->HasSpell(CREATE_SOULSTONE))
    {
        Item* soulStone = ai->FindConsumable(SOULSTONE_DISPLAYID);
        if (!soulStone)
        {
            shardCount = m_bot->GetItemCount(SOUL_SHARD, false, NULL);
            if (shardCount > 0 && !m_bot->HasSpellCooldown(CREATE_SOULSTONE) && ai->CastSpell(CREATE_SOULSTONE))
                return;
        }
        else
        {
            uint32 soulStoneSpell = soulStone->GetProto()->Spells[0].SpellId;
            if (!m_master->HasAura(soulStoneSpell) && !m_bot->HasSpellCooldown(soulStoneSpell))
            {
                ai->UseItem(soulStone, m_master);
                return;
            }
        }
    }
}
