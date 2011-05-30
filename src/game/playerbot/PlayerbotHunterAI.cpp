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
    FEIGN_DEATH                   = ai->initSpell(FEIGN_DEATH_1, true);

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
    ASPECT_OF_THE_VIPER           = ai->initSpell(ASPECT_OF_THE_VIPER_1, true);
    ASPECT_OF_THE_DRAGONHAWK      = ai->initSpell(ASPECT_OF_THE_DRAGONHAWK_1, true);
    RAPID_FIRE                    = ai->initSpell(RAPID_FIRE_1);
    TRUESHOT_AURA                 = ai->initSpell(TRUESHOT_AURA_1, true);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_HUNTER); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    BLOOD_FURY                    = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP                     = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING                    = ai->initSpell(BERSERKING_ALL); // troll
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
} // end HasPet

void PlayerbotHunterAI::DoCombatManeuver(Unit *pTarget)
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

    // Hunter
    Unit* pVictim = pTarget->getVictim();

    // check for pet and heal if neccessary
    Pet *pet = m_bot->GetPet();
    if ((pet)
        && (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f)
        && (PET_MEND > 0 && !pet->getDeathState() != ALIVE && pVictim != m_bot && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && ai->GetManaPercent() >= 13 && ai->CastSpell(PET_MEND, m_bot)))
    {

        return;
    }
    else if ((pet)
             && (INTIMIDATION > 0 && pVictim == pet && !pet->HasAura(INTIMIDATION, EFFECT_INDEX_0) && ai->CastSpell(INTIMIDATION, m_bot)))

        return;

    // racial traits
    if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0) && ai->CastSpell(BLOOD_FURY, m_bot))
        return;
    else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0) && ai->CastSpell(BERSERKING, m_bot))
        return;

    if (m_bot->IsWithinDist(pTarget, ATTACK_DISTANCE))
        m_rangedCombat = false;
    else
        m_rangedCombat = true;

    if (!m_rangedCombat)
    {
        if (pTarget->getVictim() == m_bot)
        {
            if (FEIGN_DEATH && !m_bot->HasAura(FEIGN_DEATH) && ai->CastSpell(FEIGN_DEATH))
                return;
        }
        else if (!m_bot->isMoving() && !m_bot->hasUnitState(UNIT_STAT_NO_FREE_MOVE) && !m_bot->hasUnitState(UNIT_STAT_CONTROLLED))
        {
            float xb, yb, zb, xt, yt, zt, xbt, ybt, angle, offset_x, offset_y;
            m_bot->GetPosition(xb,yb,zb);
            pTarget->GetPosition(xt,yt,zt);
            angle = atan(abs(xb - xt) / abs(yb - yt));
            offset_x = ATTACK_DISTANCE * 2 * sin(angle);
            offset_y = ATTACK_DISTANCE * 2 * cos(angle);

            xbt = (xb < xt) ? (xt - offset_x) : (xt + offset_x);
            ybt = (yb < yt) ? (yt - offset_y) : (yt + offset_y);

            const TerrainInfo *map = m_bot->GetTerrain();

            if (map->GetHeight(xbt, ybt, zb + 2.0f, true) > (zb - 10.0f) && map->GetHeight(xbt, ybt, zb + 2.0f, true) < (zb + 5.0f) && pTarget->IsWithinLOS(xbt, ybt, map->GetHeight(xbt, ybt, zb + 2.0f, true)))
            {
                m_bot->AttackStop();
                m_bot->GetMotionMaster()->MovePoint(m_master->GetMapId(), xbt, ybt, map->GetHeight(xbt, ybt, zb + 2.0f, true));
            }
            else
            {
                xbt = (xb < xt) ? (xt + offset_x) : (xt - offset_x);
                ybt = (yb < yt) ? (yt + offset_y) : (yt - offset_y);

                if (map->GetHeight(xbt, ybt, zb + 2.0f, true) > (zb - 10.0f) && map->GetHeight(xbt, ybt, zb + 2.0f, true) < (zb + 5.0f) && pTarget->IsWithinLOS(xbt, ybt, map->GetHeight(xbt, ybt, zb + 2.0f, true)))
                {
                    m_bot->AttackStop();
                    m_bot->GetMotionMaster()->MovePoint(m_master->GetMapId(), xbt, ybt, map->GetHeight(xbt, ybt, zb + 2.0f, true));
                }
            }

            return;
        }
    }

    if (!m_bot->isInFront(pTarget, 50.0f, 0) && !m_bot->hasUnitState(UNIT_STAT_NO_FREE_MOVE) && !m_bot->hasUnitState(UNIT_STAT_CONTROLLED))
        ai->SetInFront(pTarget);

    if (!m_bot->HasAura(ASPECT_OF_THE_VIPER) && ai->GetManaPercent() < 50 && ai->CastSpell(ASPECT_OF_THE_VIPER, m_bot))
        return;
    else if (!m_bot->HasAura(ASPECT_OF_THE_DRAGONHAWK) && ai->GetManaPercent() >= 80 && ai->CastSpell(ASPECT_OF_THE_DRAGONHAWK, m_bot))
        return;

    // activate auto shot
    if (!m_bot->FindCurrentSpellBySpellId(AUTO_SHOT))
        ai->CastSpell(AUTO_SHOT, pTarget);
    else if (m_bot->FindCurrentSpellBySpellId(AUTO_SHOT))
        m_bot->InterruptNonMeleeSpells(true, AUTO_SHOT);

    // damage spells

    if (!pTarget->HasAura(HUNTERS_MARK) && ai->CastSpell(HUNTERS_MARK, pTarget))
        return;

    if (m_bot->HasAura(ASPECT_OF_THE_VIPER) && ai->CastSpell(RAPID_FIRE))
        return;

    static const uint32 RangedSpell[] = {KILL_SHOT, EXPLOSIVE_SHOT, SERPENT_STING, BLACK_ARROW, ARCANE_SHOT, AIMED_SHOT, STEADY_SHOT};
    static const uint32 MeleeSpell[] = {MONGOOSE_BITE, RAPTOR_STRIKE, IMMOLATION_TRAP, EXPLOSIVE_TRAP};
    static const uint32 eltRanged = sizeof(RangedSpell)/sizeof(uint32);
    static const uint32 eltMelee = sizeof(MeleeSpell)/sizeof(uint32);
    char *RangedSpellEnabled = "001110F";
    char *MeleeSpellEnabled = "1110";
    uint32 numberTargets = 0;

    if (m_bot->getLevel() >= 34)
        MeleeSpellEnabled = "1101";

    if (m_bot->getLevel() >= 60)
        RangedSpellEnabled = "111101F";

    // Count number of targets
    Group* m_group = m_bot->GetGroup();
    GroupReference *ref = (m_group) ? m_group->GetFirstMember() : NULL;
    do
    {
        Player *g_member = (ref) ? ref->getSource() : m_bot;

        if (!g_member->isAlive())
            continue;

        for (Unit::AttackerSet::const_iterator itr = g_member->getAttackers().begin(); itr != g_member->getAttackers().end(); itr++)
            if ((*itr)->IsWithinDist(m_bot, 36.0f))
                numberTargets++;

    }while(ref = (ref) ? ref->next() : NULL);

    // Spells with Area of Effect
    if (m_rangedCombat && numberTargets >= 3 && ai->CastSpell(MULTI_SHOT, pTarget))
        return;

    if (numberTargets >= 5 && ai->CastSpell(VOLLEY, pTarget))
        return;

    // Casting spell cycle
    if (m_rangedCombat)
    {
        for (uint32 i = 0; i < eltRanged; ++i)
        {
            if (RangedSpellEnabled[i] == 'F')
            {
                if (m_bot->HasAura(ASPECT_OF_THE_DRAGONHAWK) && !pTarget->HasAuraFromUnit(RangedSpell[i], m_bot) && ai->CastSpell(RangedSpell[i], pTarget))
                    return;
                else
                    continue;
            }
            else if (RangedSpellEnabled[i] == '1')
            {
                if (!pTarget->HasAuraFromUnit(RangedSpell[i], m_bot) && ai->CastSpell(RangedSpell[i], pTarget))
                    return;
            }
        }
    }
    else
    {
        for (uint32 i = 0; i < eltMelee; ++i)
        {
            if (!pTarget->HasAuraFromUnit(MeleeSpell[i], m_bot) && ai->CastSpell(MeleeSpell[i], pTarget))
                return;
        }
    }

} // end DoCombatManeuver

void PlayerbotHunterAI::DoNonCombatActions()
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

    static const uint32 MinorGlyphs[] = {57903, 57866, 57870};           // feigh death - revive pet - healing pet
    static const uint32 SurvivalMajorGlyphs[] = {56832, 56826, 63066};     // serpent sting - steady shot - explosive shot

    for (uint32 i = 0; i < 3; i++)
    {
        if (!m_bot->HasSpell(MinorGlyphs[i]))
            m_bot->learnSpell(MinorGlyphs[i], true);

        if (!m_bot->HasSpell(SurvivalMajorGlyphs[i]))
            m_bot->learnSpell(SurvivalMajorGlyphs[i], true);
    }

    if (!m_bot->HasAura(TRUESHOT_AURA, EFFECT_INDEX_0))
        ai->CastSpell(TRUESHOT_AURA, m_bot);

    if (!m_bot->HasAura(ASPECT_OF_THE_VIPER, EFFECT_INDEX_0) && ai->GetManaPercent() < 90 && ai->CastSpell(ASPECT_OF_THE_VIPER, m_bot))
        return;
    else if (!m_bot->HasAura(ASPECT_OF_THE_DRAGONHAWK, EFFECT_INDEX_0) && ai->GetManaPercent() >= 90 && ai->CastSpell(ASPECT_OF_THE_DRAGONHAWK, m_bot))
        return;

    // reset ranged combat state
    if (!m_rangedCombat)
        m_rangedCombat = true;

    // check for pet
    if (PET_SUMMON > 0 && !m_petSummonFailed && HasPet(m_bot))
    {
        // we can summon pet, and no critical summon errors before
        Pet *pet = m_bot->GetPet();
        if (!pet)
        {
            // summon pet
            if (PET_SUMMON > 0 && ai->CastSpell(PET_SUMMON, m_bot))
            {
            }
            else
            {
                m_petSummonFailed = true;
            }
        }
        else if (pet->getDeathState() != ALIVE)
        {
            // revive pet
            if (PET_REVIVE > 0 && ai->GetManaPercent() >= 80 && ai->CastSpell(PET_REVIVE, m_bot))
            {
            }
        }
        else if (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f)
        {
            // heal pet when health lower 50%
            if (PET_MEND > 0 && !pet->getDeathState() != ALIVE && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && ai->GetManaPercent() >= 13 && ai->CastSpell(PET_MEND, m_bot))
            {
            }
        }
        else if (pet->GetHappinessState() != HAPPY) // if pet is hungry
        {
            Unit *caster = (Unit*) m_bot;
            // list out items in main backpack
            for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
            {
                Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                    {
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
            // list out items in other removable backpacks
            for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
            {
                const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                if (pBag)
                    for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                    {
                        Item* const pItem = m_bot->GetItemByPos(bag, slot);
                        if (pItem)
                        {
                            const ItemPrototype* const pItemProto = pItem->GetProto();
                            if (!pItemProto)
                                continue;

                            if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                            {
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
            }
            if (pet->HasAura(PET_MEND, EFFECT_INDEX_0) && !pet->HasAura(PET_FEED, EFFECT_INDEX_0))

            ai->SetIgnoreUpdateTime(7);
        }
    }
} // end DoNonCombatActions
