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

class PlayerbotAI;

PlayerbotHunterAI::PlayerbotHunterAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
    m_petSummonFailed = false;
    m_rangedCombat = true;
}

PlayerbotHunterAI::~PlayerbotHunterAI() {}

void PlayerbotHunterAI::InitSpells(PlayerbotAI* const ai)
{
    PET_SUMMON = ai->getSpellId("call pet");
    PET_DISMISS = ai->getSpellId("dismiss pet");
    PET_REVIVE = ai->getSpellId("revive pet");
    PET_MEND = ai->getSpellId("mend pet");
    PET_FEED = 1539;
    INTIMIDATION = ai->getSpellId("intimidation");
    SONIC_BLAST = ai->getSpellId("sonic blast");
    DEMORALIZING_SCREECH = ai->getSpellId("demoralizing screech");
    BAD_ATTITUDE = ai->getSpellId("bad attitude");
    NETHER_SHOCK = ai->getSpellId("nether shock");
    AUTO_SHOT = ai->getSpellId("auto shot");
    HUNTERS_MARK = ai->getSpellId("hunter's mark");
    ARCANE_SHOT = ai->getSpellId("arcane shot");
    CONCUSSIVE_SHOT = ai->getSpellId("concussive shot");
    DISTRACTING_SHOT = ai->getSpellId("distracting shot");
    MULTI_SHOT = ai->getSpellId("multi-shot");
    EXPLOSIVE_SHOT = ai->getSpellId("explosive shot");
    SERPENT_STING = ai->getSpellId("serpent sting");
    SCORPID_STING = ai->getSpellId("scorpid sting");
    WYVERN_STING = ai->getSpellId("wyvern sting");
    VIPER_STING = ai->getSpellId("viper sting");
    AIMED_SHOT = ai->getSpellId("aimed shot");
    STEADY_SHOT = ai->getSpellId("steady shot");
    CHIMERA_SHOT = ai->getSpellId("chimera shot");
    VOLLEY = ai->getSpellId("volley");
    BLACK_ARROW = ai->getSpellId("black arrow");
    KILL_SHOT = ai->getSpellId("kill shot");
    RAPTOR_STRIKE = ai->getSpellId("raptor strike");
    WING_CLIP = ai->getSpellId("wing clip");
    MONGOOSE_BITE = ai->getSpellId("mongoose bite");
    DISENGAGE = ai->getSpellId("disengage");
    MISDIRECTION = ai->getSpellId("misdirection");
    DETERRENCE = ai->getSpellId("deterrence");
    BEAR_TRAP = ai->getSpellId("bear trap");
    FREEZING_TRAP = ai->getSpellId("freezing trap");
    IMMOLATION_TRAP = ai->getSpellId("immolation trap");
    FROST_TRAP = ai->getSpellId("frost trap");
    EXPLOSIVE_TRAP = ai->getSpellId("explosive trap");
    ARCANE_TRAP = ai->getSpellId("arcane trap");
    SNAKE_TRAP = ai->getSpellId("snake trap");
    ASPECT_OF_THE_HAWK = ai->getSpellId("aspect of the hawk");
    ASPECT_OF_THE_MONKEY = ai->getSpellId("aspect of the monkey");
    RAPID_FIRE = ai->getSpellId("rapid fire");
    TRUESHOT_AURA = 19506;
    RECENTLY_BANDAGED = 11196;
    ARCANE_TORRENT = ai->getSpellId("arcane torrent");
    GIFT_OF_THE_NAARU = ai->getSpellId("gift of the naaru");
    STONEFORM = ai->getSpellId("stoneform");
    SHADOWMELD = ai->getSpellId("shadowmeld");
    BLOOD_FURY = ai->getSpellId("blood fury");
    WAR_STOMP = ai->getSpellId("war stomp");
    BERSERKING = ai->getSpellId("berserking");
}

bool PlayerbotHunterAI::HasPet(Player* bot)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM character_pet WHERE owner = '%u' AND(slot = '%u' OR slot = '%u')", bot->GetGUIDLow(), PET_SAVE_AS_CURRENT, PET_SAVE_NOT_IN_SLOT);

    if (result)
        return true;
    else
        return false;
}

bool PlayerbotHunterAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();

    Pet *pet = m_bot->GetPet();
    if (pet
        && (((float)pet->GetHealth() / (float)pet->GetMaxHealth()) < 0.5f)
        && (PET_MEND > 0)
        && (!pet->getDeathState() != ALIVE)
        && (pVictim != m_bot)
        && (!pet->HasAura(PET_MEND, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 13)
        && (ai->CastSpell(PET_MEND, *m_bot)))
        return true;

    if (pet
        && (INTIMIDATION > 0)
        && (pVictim == pet)
        && (!pet->HasAura(INTIMIDATION, EFFECT_INDEX_0))
        && (ai->CastSpell(INTIMIDATION, *m_bot)))
        return true;

    if ((m_bot->getRace() == RACE_ORC)
        && (!m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0))
        && (ai->CastSpell(BLOOD_FURY, *m_bot)))
        return true;

    if ((m_bot->getRace() == RACE_TROLL)
        && (!m_bot->HasAura(BERSERKING, EFFECT_INDEX_0))
        && (ai->CastSpell(BERSERKING, *m_bot)))
        return true;

    float dist = m_bot->GetDistance(pTarget);

    if (((dist <= ATTACK_DISTANCE || !m_bot->GetUInt32Value(PLAYER_AMMO_ID)))
        && (m_rangedCombat))
    {
        m_rangedCombat = false;
        if ((ASPECT_OF_THE_MONKEY > 0)
            && (!m_bot->HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0))
            && (ai->CastSpell(ASPECT_OF_THE_MONKEY, *m_bot)))
            return true;

        return false;
    }

    if ((dist > ATTACK_DISTANCE)
        && (!m_rangedCombat))
    {
        m_rangedCombat = true;
        if ((ASPECT_OF_THE_HAWK > 0)
            && (!m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0))
            && (ai->CastSpell(ASPECT_OF_THE_HAWK, *m_bot)))
            return true;

        return false;
    }

    if ((m_rangedCombat)
        && (!m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0)))
    {
        if ((ASPECT_OF_THE_HAWK > 0)
            && (ai->CastSpell(ASPECT_OF_THE_HAWK, *m_bot)))
            return true;

        return false;
    }

    if ((!m_rangedCombat)
        && (!m_bot->HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0)))
    {
        if ((ASPECT_OF_THE_MONKEY > 0)
            && (ai->CastSpell(ASPECT_OF_THE_MONKEY, *m_bot)))
            return true;

        return false;
    }

    if ((AUTO_SHOT > 0)
        && (m_rangedCombat)
        && (!m_bot->FindCurrentSpellBySpellId(AUTO_SHOT))
        && (ai->CastSpell(AUTO_SHOT, *pTarget)))
        return true;

    if ((AUTO_SHOT > 0)
        && (m_bot->FindCurrentSpellBySpellId(AUTO_SHOT)))
    {
        m_bot->InterruptNonMeleeSpells(true, AUTO_SHOT);
        return true;
    }

    if (m_rangedCombat)
    {
        if ((HUNTERS_MARK > 0)
            && (ai->GetManaPercent() >= 3)
            && (!pTarget->HasAura(HUNTERS_MARK, EFFECT_INDEX_0))
            && (ai->CastSpell(HUNTERS_MARK, *pTarget)))
            return true;

        if ((RAPID_FIRE > 0)
        && (ai->GetManaPercent() >= 3)
            && (!m_bot->HasAura(RAPID_FIRE, EFFECT_INDEX_0))
            && (ai->CastSpell(RAPID_FIRE, *m_bot)))
            return true;

        if ((MULTI_SHOT > 0)
            && (ai->GetManaPercent() >= 13)
            && (ai->GetAttackerCount() >= 3)
            && (ai->CastSpell(MULTI_SHOT, *pTarget)))
            return true;

        if ((ARCANE_SHOT > 0)
            && (ai->GetManaPercent() >= 7)
            && (ai->CastSpell(ARCANE_SHOT, *pTarget)))
            return true;

        if ((CONCUSSIVE_SHOT > 0)
            && (ai->GetManaPercent() >= 6)
            && (!pTarget->HasAura(CONCUSSIVE_SHOT, EFFECT_INDEX_0))
            && (ai->CastSpell(CONCUSSIVE_SHOT, *pTarget)))
            return true;

        if ((EXPLOSIVE_SHOT > 0)
            && (ai->GetManaPercent() >= 10)
            && (!pTarget->HasAura(EXPLOSIVE_SHOT, EFFECT_INDEX_0))
            && (ai->CastSpell(EXPLOSIVE_SHOT, *pTarget)))
            return true;

        if ((VIPER_STING > 0)
            && (ai->GetManaPercent() >= 8)
            && (pTarget->GetPower(POWER_MANA) > 0)
            && (ai->GetManaPercent() < 70)
            && (!pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0))
            && (ai->CastSpell(VIPER_STING, *pTarget)))
            return true;

        if ((SERPENT_STING > 0)
            && (ai->GetManaPercent() >= 13)
            && (!pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0))
            && (!pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0))
            && (!pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0))
            && (ai->CastSpell(SERPENT_STING, *pTarget)))
            return true;

        if ((SCORPID_STING > 0)
            && (ai->GetManaPercent() >= 11)
            && (!pTarget->HasAura(WYVERN_STING, EFFECT_INDEX_0))
            && (!pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0))
            && (!pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0))
            && (!pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0))
            && (ai->CastSpell(SCORPID_STING, *pTarget)))
            return true;

        if ((CHIMERA_SHOT > 0)
            && (ai->GetManaPercent() >= 12)
            && (ai->CastSpell(CHIMERA_SHOT, *pTarget)))
            return true;

        if ((VOLLEY > 0)
            && (ai->GetManaPercent() >= 24)
            && (ai->GetAttackerCount() >= 3)
            && (ai->CastSpell(VOLLEY, *pTarget)))
            return true;

        if ((BLACK_ARROW > 0)
            && (ai->GetManaPercent() >= 6)
            && (!pTarget->HasAura(BLACK_ARROW, EFFECT_INDEX_0))
            && (ai->CastSpell(BLACK_ARROW, *pTarget)))
            return true;

        if ((AIMED_SHOT > 0)
            && (ai->GetManaPercent() >= 12)
            && (ai->CastSpell(AIMED_SHOT, *pTarget)))
            return true;

        if ((STEADY_SHOT > 0)
            && (ai->GetManaPercent() >= 5)
            && (ai->CastSpell(STEADY_SHOT, *pTarget)))
            return true;

        if ((KILL_SHOT > 0)
            && (ai->GetManaPercent() >= 7)
            && (pTarget->GetHealth() < pTarget->GetMaxHealth() *0.2)
            && (ai->CastSpell(KILL_SHOT, *pTarget)))
            return true;
    }
    else
    {
        if ((RAPTOR_STRIKE > 0)
            && (ai->GetManaPercent() >= 6)
            && (ai->CastSpell(RAPTOR_STRIKE, *pTarget)))
            return true;

        if ((EXPLOSIVE_TRAP > 0)
            && (ai->GetManaPercent() >= 27)
            && (!pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0))
            && (ai->CastSpell(EXPLOSIVE_TRAP, *pTarget)))
            return true;

        if ((WING_CLIP > 0)
            && (ai->GetManaPercent() >= 6)
            && (!pTarget->HasAura(WING_CLIP, EFFECT_INDEX_0))
            && (ai->CastSpell(WING_CLIP, *pTarget)))
            return true;

        if ((IMMOLATION_TRAP > 0)
            && (ai->GetManaPercent() >= 13)
            && (!pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0))
            && (ai->CastSpell(IMMOLATION_TRAP, *pTarget)))
            return true;

        if ((MONGOOSE_BITE > 0)
            && (ai->GetManaPercent() >= 4)
            && (ai->CastSpell(MONGOOSE_BITE, *pTarget)))
            return true;

        if ((FROST_TRAP > 0)
            && (ai->GetManaPercent() >= 2)
            && (!pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0))
            && (ai->CastSpell(FROST_TRAP, *pTarget)))
            return true;

        if ((ARCANE_TRAP > 0)
            && (!pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0))
            && (!pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0))
            && (ai->CastSpell(ARCANE_TRAP, *pTarget)))
            return true;

        if ((DETERRENCE > 0)
            && (pVictim == m_bot)
            && (m_bot->GetHealth() < m_bot->GetMaxHealth() *0.5)
            && (!m_bot->HasAura(DETERRENCE, EFFECT_INDEX_0))
            && (ai->CastSpell(DETERRENCE, *m_bot)))
            return true;

        if ((m_bot->getRace() == RACE_TAUREN)
            && (!pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0))
            && (ai->CastSpell(WAR_STOMP, *pTarget)))
            return true;

        if ((m_bot->getRace() == RACE_BLOODELF)
            && (!pTarget->HasAura(ARCANE_TORRENT, EFFECT_INDEX_0))
            && (ai->CastSpell(ARCANE_TORRENT, *pTarget)))
            return true;

        if ((m_bot->getRace() == RACE_DWARF)
            && (m_bot->HasAuraState(AURA_STATE_DEADLY_POISON))
            && (ai->CastSpell(STONEFORM, *m_bot)))
            return true;

        if ((m_bot->getRace() == RACE_NIGHTELF)
            && (pVictim == m_bot)
            && (ai->GetHealthPercent() < 25)
            && (!m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0))
            && (ai->CastSpell(SHADOWMELD, *m_bot)))
            return true;

        if ((m_bot->getRace() == RACE_DRAENEI)
            && (ai->GetHealthPercent() < 25)
            && (!m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0))
            && (ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot)))
            return true;

        if (pet
            && (!pet->getDeathState() != ALIVE)
            && (MISDIRECTION > 0)
            && (pVictim == m_bot)
            && (!m_bot->HasAura(MISDIRECTION, EFFECT_INDEX_0))
            && (ai->GetManaPercent() >= 9)
            && (ai->CastSpell(MISDIRECTION, *pet)))
            return true;
    }

    return false;
}

bool PlayerbotHunterAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    if (!m_rangedCombat)
        m_rangedCombat = true;

    if ((TRUESHOT_AURA > 0)
        && (!m_bot->HasAura(TRUESHOT_AURA, EFFECT_INDEX_0))
        && (ai->CastSpell(TRUESHOT_AURA, *m_bot)))
        return true;

    if ((ASPECT_OF_THE_HAWK > 0)
        && (!m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0))
        && (ai->CastSpell(ASPECT_OF_THE_HAWK, *m_bot)))
        return true;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* drinkOrFoodItem = ai->FindDrink();
    Item* bandageItem = ai->FindBandage();

    if (drinkOrFoodItem
        && (ai->GetManaPercent() < 30))
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    drinkOrFoodItem = ai->FindFood();

    if (drinkOrFoodItem
        && (ai->GetHealthPercent() < 30))
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    if (!drinkOrFoodItem
        && bandageItem
        && (!m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0))
        && (ai->GetHealthPercent() < 70))
    {
        ai->UseItem(*bandageItem);
        ai->SetIgnoreUpdateTime(8);
        return true;
    }

    if (!drinkOrFoodItem
        && !bandageItem
        && (m_bot->getRace() == RACE_DRAENEI)
        && (!m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0))
        && (ai->GetHealthPercent() < 70))
    {
        ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot);
        return true;
    }

    if ((PET_SUMMON > 0)
        && (!m_petSummonFailed)
        && (HasPet(m_bot)))
    {
        Pet *pet = m_bot->GetPet();
        if (!pet)
        {
            if ((PET_SUMMON > 0)
                && (ai->CastSpell(PET_SUMMON, *m_bot)))
                return true;

            m_petSummonFailed = true;
            return false;
        }

        if (pet->getDeathState() != ALIVE)
        {
            if ((PET_REVIVE > 0)
                && (ai->GetManaPercent() >= 80)
                && (ai->CastSpell(PET_REVIVE, *m_bot)))
                return true;

            return false;
        }

        if (((float)pet->GetHealth() / (float)pet->GetMaxHealth()) < 0.5f)
        {
            if ((PET_MEND > 0)
                && (!pet->getDeathState() != ALIVE)
                && (!pet->HasAura(PET_MEND, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 13)
                && (ai->CastSpell(PET_MEND, *m_bot)))
                return true;

            return false;
        }

        if (pet->GetHappinessState() != HAPPY)
        {
            Unit *caster = (Unit*)m_bot;
            for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
            {
                Item* const drinkOrFoodItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                if (drinkOrFoodItem)
                {
                    const ItemPrototype* const drinkOrFoodItemProto = drinkOrFoodItem->GetProto();
                    if (!drinkOrFoodItemProto)
                        continue;

                    if (pet->HaveInDiet(drinkOrFoodItemProto))
                    {
                        caster->CastSpell(caster, 51284, true);
                        uint32 count = 1;
                        int32 benefit = pet->GetCurrentFoodBenefitLevel(drinkOrFoodItemProto->ItemLevel);
                        m_bot->DestroyItemCount(drinkOrFoodItem, count, true);
                        m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true);
                        ai->SetIgnoreUpdateTime(10);
                        return true;
                    }
                }
            }

            for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
            {
                const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                if (pBag)
                {
                    for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                    {
                        Item* const drinkOrFoodItem = m_bot->GetItemByPos(bag, slot);
                        if (drinkOrFoodItem)
                        {
                            const ItemPrototype* const drinkOrFoodItemProto = drinkOrFoodItem->GetProto();
                            if (!drinkOrFoodItemProto)
                                continue;

                            if (pet->HaveInDiet(drinkOrFoodItemProto))
                            {
                                caster->CastSpell(caster, 51284, true);
                                uint32 count = 1;
                                int32 benefit = pet->GetCurrentFoodBenefitLevel(drinkOrFoodItemProto->ItemLevel);
                                m_bot->DestroyItemCount(drinkOrFoodItem, count, true);
                                m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true);
                                ai->SetIgnoreUpdateTime(10);
                                return true;
                            }
                        }
                    }
                }
            }

            if ((pet->HasAura(PET_MEND, EFFECT_INDEX_0))
                && (!pet->HasAura(PET_FEED, EFFECT_INDEX_0)))
            {
                ai->SetIgnoreUpdateTime(7);
                return true;
            }
        }
    }

    return false;
}
