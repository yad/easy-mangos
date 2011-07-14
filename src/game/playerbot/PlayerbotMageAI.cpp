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

#include "PlayerbotMageAI.h"

class PlayerbotAI;

PlayerbotMageAI::PlayerbotMageAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
    LastSpellFrost = 0;
    LastSpellFire = 0;
    LastSpellArcane = 0;
}

void PlayerbotMageAI::ReinitCycles()
{
    LastSpellFrost = 0;
    LastSpellFire = 0;
    LastSpellArcane = 0;
}

void PlayerbotMageAI::InitSpells(PlayerbotAI* const ai)
{
    ARCANE_MISSILES         = ai->initSpell(ARCANE_MISSILES_1);
    ARCANE_EXPLOSION        = ai->initSpell(ARCANE_EXPLOSION_1);
    COUNTERSPELL            = ai->initSpell(COUNTERSPELL_1);
    SLOW                    = ai->initSpell(SLOW_1);
    ARCANE_BARRAGE          = ai->initSpell(ARCANE_BARRAGE_1);
    ARCANE_BLAST            = ai->initSpell(ARCANE_BLAST_1);
    ARCANE_POWER            = ai->initSpell(ARCANE_POWER_1);
    DAMPEN_MAGIC            = ai->initSpell(DAMPEN_MAGIC_1);
    AMPLIFY_MAGIC           = ai->initSpell(AMPLIFY_MAGIC_1);
    MAGE_ARMOR              = ai->initSpell(MAGE_ARMOR_1);
    MIRROR_IMAGE            = ai->initSpell(MIRROR_IMAGE_1);
    ARCANE_INTELLECT        = ai->initSpell(ARCANE_INTELLECT_1);
    ARCANE_BRILLIANCE       = ai->initSpell(ARCANE_BRILLIANCE_1);
    DALARAN_INTELLECT       = ai->initSpell(DALARAN_INTELLECT_1);
    DALARAN_BRILLIANCE      = ai->initSpell(DALARAN_BRILLIANCE_1);
    MANA_SHIELD             = ai->initSpell(MANA_SHIELD_1);
    CONJURE_WATER           = ai->initSpell(CONJURE_WATER_1);
    CONJURE_FOOD            = ai->initSpell(CONJURE_FOOD_1);
    FIREBALL                = ai->initSpell(FIREBALL_1);
    FIRE_BLAST              = ai->initSpell(FIRE_BLAST_1);
    FLAMESTRIKE             = ai->initSpell(FLAMESTRIKE_1);
    SCORCH                  = ai->initSpell(SCORCH_1);
    PYROBLAST               = ai->initSpell(PYROBLAST_1);
    BLAST_WAVE              = ai->initSpell(BLAST_WAVE_1);
    COMBUSTION              = ai->initSpell(COMBUSTION_1);
    DRAGONS_BREATH          = ai->initSpell(DRAGONS_BREATH_1);
    LIVING_BOMB             = ai->initSpell(LIVING_BOMB_1);
    FROSTFIRE_BOLT          = ai->initSpell(FROSTFIRE_BOLT_1);
    FIRE_WARD               = ai->initSpell(FIRE_WARD_1);
    MOLTEN_ARMOR            = ai->initSpell(MOLTEN_ARMOR_1);
    ICY_VEINS               = ai->initSpell(ICY_VEINS_1);
    DEEP_FREEZE             = ai->initSpell(DEEP_FREEZE_1);
    FROSTBOLT               = ai->initSpell(FROSTBOLT_1);
    FROST_NOVA              = ai->initSpell(FROST_NOVA_1);
    BLIZZARD                = ai->initSpell(BLIZZARD_1);
    CONE_OF_COLD            = ai->initSpell(CONE_OF_COLD_1);
    ICE_BARRIER             = ai->initSpell(ICE_BARRIER_1);
    SUMMON_WATER_ELEMENTAL  = ai->initSpell(SUMMON_WATER_ELEMENTAL_1);
    FROST_WARD              = ai->initSpell(FROST_WARD_1);
    ICE_LANCE               = ai->initSpell(ICE_LANCE_1);
    FROST_ARMOR             = ai->initSpell(FROST_ARMOR_1);
    ICE_ARMOR               = ai->initSpell(ICE_ARMOR_1);
    ICE_BLOCK               = ai->initSpell(ICE_BLOCK_1);
    COLD_SNAP               = ai->initSpell(COLD_SNAP_1);
    RECENTLY_BANDAGED       = 11196;
    ARCANE_TORRENT          = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_MAGE);
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL);
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL);
    BERSERKING              = ai->initSpell(BERSERKING_ALL);
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL);
}

PlayerbotMageAI::~PlayerbotMageAI() {}

bool PlayerbotMageAI::DoEvadeAction()
{
    PlayerbotAI *ai = GetAI();
    Player *m_bot = GetPlayerBot();

    if (ai->Cast(ICE_BLOCK, m_bot))
        return true;

    return false;
}

bool PlayerbotMageAI::DoProtectSelfAction()
{
    PlayerbotAI *ai = GetAI();
    Player *m_bot = GetPlayerBot();

    static const uint32 spells[] = {ICE_BARRIER, MANA_SHIELD};
    static uint32 elt = sizeof(spells) / sizeof(uint32);

    for (uint32 i = 0; i < elt; i++)
    {
        if (!m_bot->HasAura(spells[i]) && ai->CastSpell(spells[i], m_bot))
            return true;
    }

    return false;
}

bool PlayerbotMageAI::DoCombatManeuver(Unit *pTarget, bool cac)
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    switch (m_bot->getRole())
    {
        case MageFire:
        {
            static const uint32 SpellFire[] = {SCORCH, FROSTFIRE_BOLT, FIRE_BLAST, SCORCH, FIREBALL,
                FLAMESTRIKE, SCORCH, FIREBALL, BLAST_WAVE, SCORCH, PYROBLAST, SCORCH, FIREBALL,
                LIVING_BOMB, DRAGONS_BREATH};
            static uint32 elt = sizeof(SpellFire)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                uint32 spellId = SpellFire[(i+LastSpellFire)%elt];
                if (!cac && (spellId==BLAST_WAVE||spellId==DRAGONS_BREATH))
                   continue;
                if (ai->CastSpell(spellId, pTarget))
                {
                    LastSpellFire = (i+LastSpellFire)%elt;
                    return true;
                }
            }
            break;
        }
        case MageArcane:
        {
            static const uint32 SpellArcane[] = {ARCANE_MISSILES, ARCANE_BLAST, ARCANE_BLAST, ARCANE_BLAST,
                ARCANE_BLAST, ARCANE_MISSILES, ARCANE_BARRAGE};
            static uint32 elt = sizeof(SpellArcane)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                uint32 spellId = SpellArcane[(i+LastSpellArcane)%elt];
                if (!cac && (spellId==ARCANE_BLAST))
                   continue;
                if (ai->CastSpell(spellId, pTarget))
                {
                    LastSpellArcane = (i+LastSpellArcane)%elt;
                    return true;
                }
            }
            break;
        }
        case MageFrost:
        {
            static const uint32 SpellFrost[] = {FROSTFIRE_BOLT, FROSTBOLT, FROSTBOLT, ICE_LANCE, FROSTBOLT,
                FROSTBOLT, CONE_OF_COLD, FROSTBOLT, FROSTBOLT, ICE_LANCE, FROST_NOVA, BLIZZARD};
            static uint32 elt = sizeof(SpellFrost)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                uint32 spellId = SpellFrost[(i+LastSpellFrost)%elt];
                if (!cac && (spellId==FROST_NOVA||spellId==CONE_OF_COLD))
                   continue;
                if (spellId==FROSTBOLT && !FROSTBOLT)
                   spellId = FIREBALL;
                if (ai->CastSpell(spellId, pTarget))
                {
                    LastSpellFrost = (i+LastSpellFrost)%elt;
                    return true;
                }
            }
            break;
        }
    }

    if (BuffPlayer())
        return true;
    return false;
}

void PlayerbotMageAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    static const uint32 armor[] = {FROST_ARMOR, ICE_ARMOR, MAGE_ARMOR, MOLTEN_ARMOR};
    int elt = sizeof(armor) / sizeof(uint32);
    int num;

    switch (m_bot->getRole())
    {
        case MageFire:
        {
            for (num = elt-1; num >= 0; num--)
                if (armor[num])
                    break;
            break;
        }
        case MageArcane:
        {
            for (num = elt-2; num >= 0; num--)
                if (armor[num])
                    break;
            break;
        }
        case MageFrost:
        {
            for (num = elt-3; num >= 0; num--)
                if (armor[num])
                    break;

            if (!m_bot->GetPet() && ai->CastSpell(SUMMON_WATER_ELEMENTAL, m_bot))
                return;
            break;
        }
    }

    if (ai->Cast(armor[num]))
        return;

    if (BuffPlayer())
        return;
}

bool PlayerbotMageAI::BuffPlayer()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    if (!ai->Buff(DALARAN_INTELLECT, m_bot))
        if (ai->Buff(ARCANE_INTELLECT, m_bot))
            return true;

    if (m_bot == m_master)
    {
        Unit* buffTarget = m_bot->SelectRandomPlayerToBuffHim(10.0f);
        if (buffTarget && buffTarget->getPowerType()==POWER_MANA)
            if (!ai->Buff(DALARAN_INTELLECT, buffTarget))
                if (ai->Buff(ARCANE_INTELLECT, buffTarget))
                    return true;
    }
    else if (m_bot->GetGroup())
    {
        GroupReference *ref = NULL;
        ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            Player* pl = ref->getSource();
            if (pl!=m_bot && pl->isAlive() && !pl->duel && pl->getPowerType()==POWER_MANA)
                if (!ai->Buff(DALARAN_INTELLECT, pl))
                    if (ai->Buff(ARCANE_INTELLECT, pl))
                        return true;
            ref = ref->next();
        }
        ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            Player* pl = ref->getSource();
            if (pl!=m_bot && pl->isAlive() && !pl->duel && pl->GetPet() && pl->GetPet()->getPowerType()==POWER_MANA)
                if (!ai->Buff(DALARAN_INTELLECT, pl->GetPet()))
                    if (ai->Buff(ARCANE_INTELLECT, pl->GetPet()))
                        return true;
            ref = ref->next();
        }
    }
    return false;
}
