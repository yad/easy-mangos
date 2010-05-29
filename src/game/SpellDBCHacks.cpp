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

#include "Common.h"
#include "Opcodes.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "SharedDefines.h"
#include "DBCStores.h"
#include "SpellAuraDefines.h"

void SpellMgr::ApplySpellHacks()
{
    //Shaman totem bar hack-fix
    SpellEntry *sfix1 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66842));
    sfix1->EffectImplicitTargetA[0] = TARGET_SELF;
    SpellEntry *sfix2 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66843));
    sfix2->EffectImplicitTargetA[0] = TARGET_SELF;
    SpellEntry *sfix3 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66844));
    sfix3->EffectImplicitTargetA[0] = TARGET_SELF;

    //Surge of power spells should be longer
    SpellEntry *sfix4 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57407));
    sfix4->DurationIndex = 28;
    SpellEntry *sfix5 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(60936));
    sfix5->DurationIndex = 28;

    //Lifebloom final heal
    SpellEntry *sfix6 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(33778));
    sfix6->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;

    //Spirit of Redemption has AURA_INTERRUPT_FLAG_CAST, what the...? Maybe blizz changed this flag
    SpellEntry *sfix7 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(27827));
    sfix7->AuraInterruptFlags = 0;

    //Twilight Torment - relly dunno what blizzard intended to do
    SpellEntry *sfix8 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57935));
    sfix8->AttributesEx = 0;
    sfix8->AttributesEx4 = SPELL_ATTR_EX4_NOT_STEALABLE;
    sfix8->CastingTimeIndex = 1;
    sfix8->RecoveryTime = 0;
    sfix8->procFlags = (PROC_FLAG_TAKEN_MELEE_HIT | PROC_FLAG_TAKEN_MELEE_SPELL_HIT | PROC_FLAG_TAKEN_RANGED_HIT | PROC_FLAG_TAKEN_RANGED_SPELL_HIT | PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT);
    sfix8->procChance = 100;
    sfix8->procCharges = 0;
    sfix8->rangeIndex = 1;
    sfix8->StackAmount = 0;
    sfix8->Effect[EFFECT_INDEX_1] = 0;
    sfix8->EffectDieSides[EFFECT_INDEX_1] = 0;
    sfix8->EffectBasePoints[EFFECT_INDEX_0] = -1;
    sfix8->EffectImplicitTargetA[EFFECT_INDEX_0] = 6;
    sfix8->EffectImplicitTargetA[EFFECT_INDEX_1] = 0;
    sfix8->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
    sfix8->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
    sfix8->EffectRadiusIndex[EFFECT_INDEX_0] = 0;
    sfix8->EffectRadiusIndex[EFFECT_INDEX_1] = 0;
    sfix8->EffectApplyAuraName[EFFECT_INDEX_0] = SPELL_AURA_PROC_TRIGGER_SPELL;
    sfix8->EffectApplyAuraName[EFFECT_INDEX_1] = 0;
    sfix8->EffectAmplitude[EFFECT_INDEX_0] = 0;
    sfix8->EffectAmplitude[EFFECT_INDEX_1] = 0;
    sfix8->EffectMiscValue[EFFECT_INDEX_0] = 0;,
    sfix8->EffectMiscValue[EFFECT_INDEX_1] = 0;
    sfix8->EffectMiscValueB[EFFECT_INDEX_0] = 0;
    sfix8->EffectMiscValueB[EFFECT_INDEX_1] = 0;
    sfix8->EffectTriggerSpell[EFFECT_INDEX_0] = 57988;
    sfix8->EffectTriggerSpell[EFFECT_INDEX_1] = 0;
}
