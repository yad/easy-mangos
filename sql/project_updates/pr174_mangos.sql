-- NPC
DELETE FROM creature_template WHERE entry=37994;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid_A`, `modelid_A2`, `modelid_H`, `modelid_H2`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `unk16`, `unk17`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(37994, 0, 0, 0, 0, 0, 525, 0, 525, 0, 'Water Elemental', '', '', 0, 60, 70, 2998, 2998, 3043, 3043, 20, 115, 115, 0, 1.125, 1, 0, 2, 2, 0, 1, 1, 2000, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 100, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31707, 0, 0, 0, 0, 0, 0, '', 1, 3, 0.75, 0.33, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, '');

-- Levelstats
DELETE FROM pet_levelstats WHERE creature_entry=37994;

INSERT INTO `pet_levelstats` (`creature_entry`, `level`, `hp`, `mana`, `armor`, `str`, `agi`, `sta`, `inte`, `spi`) VALUES
(37994, 1, 60, 60, 0, 20, 16, 20, 13, 8),
(37994, 2, 73, 73, 0, 21, 17, 21, 14, 10),
(37994, 3, 87, 87, 0, 22, 18, 22, 15, 12),
(37994, 4, 101, 101, 0, 23, 19, 23, 16, 14),
(37994, 5, 115, 115, 0, 24, 20, 24, 17, 16),
(37994, 6, 129, 129, 0, 25, 21, 25, 18, 18),
(37994, 7, 152, 152, 0, 26, 22, 26, 19, 20),
(37994, 8, 156, 156, 0, 27, 23, 27, 20, 22),
(37994, 9, 170, 170, 0, 28, 24, 28, 21, 24),
(37994, 10, 184, 184, 0, 29, 25, 29, 22, 26),
(37994, 11, 198, 198, 0, 30, 25, 32, 22, 28),
(37994, 12, 225, 225, 0, 31, 26, 36, 24, 31),
(37994, 13, 254, 254, 0, 32, 28, 43, 26, 32),
(37994, 14, 284, 254, 0, 33, 28, 44, 25, 35),
(37994, 15, 313, 313, 0, 34, 30, 48, 26, 37),
(37994, 16, 343, 343, 0, 36, 30, 52, 27, 39),
(37994, 17, 375, 375, 0, 38, 31, 55, 28, 42),
(37994, 18, 392, 392, 0, 39, 33, 59, 28, 44),
(37994, 19, 438, 438, 0, 40, 33, 63, 30, 46),
(37994, 20, 471, 471, 0, 42, 35, 67, 31, 49),
(37994, 21, 500, 500, 0, 44, 36, 80, 31, 51),
(37994, 22, 528, 528, 0, 46, 36, 84, 32, 53),
(37994, 23, 558, 558, 0, 47, 38, 89, 33, 55),
(37994, 24, 595, 595, 0, 49, 39, 83, 34, 58),
(37994, 25, 630, 630, 0, 50, 40, 87, 35, 61),
(37994, 26, 678, 678, 0, 52, 41, 91, 36, 62),
(37994, 27, 705, 705, 0, 53, 42, 95, 37, 65),
(37994, 28, 740, 740, 0, 55, 43, 99, 38, 67),
(37994, 29, 775, 775, 0, 56, 44, 103, 39, 70),
(37994, 30, 800, 800, 0, 58, 46, 107, 40, 72),
(37994, 31, 856, 856, 0, 59, 47, 111, 40, 74),
(37994, 32, 921, 921, 0, 61, 48, 115, 41, 77),
(37994, 33, 987, 987, 0, 62, 49, 119, 42, 79),
(37994, 34, 1047, 1047, 0, 63, 51, 123, 43, 82),
(37994, 35, 1107, 1107, 0, 65, 52, 127, 44, 84),
(37994, 36, 1167, 1167, 0, 66, 53, 131, 45, 87),
(37994, 37, 1227, 1227, 0, 68, 53, 135, 46, 89),
(37994, 38, 1288, 1288, 0, 70, 55, 139, 47, 91),
(37994, 39, 1348, 1348, 0, 72, 56, 143, 48, 94),
(37994, 40, 1408, 1408, 0, 74, 58, 148, 49, 97),
(37994, 41, 1468, 1468, 0, 76, 59, 152, 50, 99),
(37994, 42, 1528, 1528, 0, 81, 59, 156, 51, 102),
(37994, 43, 1588, 1588, 0, 86, 61, 160, 52, 104),
(37994, 44, 1648, 1648, 0, 89, 62, 164, 53, 107),
(37994, 45, 1708, 1708, 0, 91, 64, 169, 54, 109),
(37994, 46, 1768, 1768, 0, 94, 65, 173, 55, 112),
(37994, 47, 1828, 1828, 0, 96, 66, 178, 56, 115),
(37994, 48, 1889, 1889, 0, 99, 68, 182, 57, 117),
(37994, 49, 1950, 1950, 0, 101, 69, 186, 58, 120),
(37994, 50, 2003, 2003, 0, 104, 71, 191, 59, 123),
(37994, 51, 2055, 2055, 0, 107, 72, 195, 61, 126),
(37994, 52, 2108, 2108, 0, 109, 73, 199, 62, 128),
(37994, 53, 2161, 2161, 0, 112, 75, 204, 63, 131),
(37994, 54, 2213, 2213, 0, 114, 76, 208, 64, 134),
(37994, 55, 2266, 2266, 0, 117, 77, 213, 65, 136),
(37994, 56, 2318, 2318, 0, 119, 79, 217, 66, 139),
(37994, 57, 2371, 2371, 0, 122, 80, 221, 67, 142),
(37994, 58, 2424, 2425, 0, 124, 82, 226, 68, 144),
(37994, 59, 2476, 2476, 0, 127, 83, 230, 69, 147),
(37994, 60, 2529, 1874, 0, 129, 85, 234, 70, 150),
(37994, 61, 2582, 2682, 0, 130, 86, 238, 71, 153),
(37994, 62, 2635, 2735, 0, 131, 87, 242, 72, 156),
(37994, 63, 2688, 2788, 0, 132, 88, 246, 73, 159),
(37994, 64, 2741, 2841, 0, 133, 89, 250, 74, 162),
(37994, 65, 2794, 2894, 0, 134, 90, 254, 75, 165),
(37994, 66, 2847, 2947, 0, 135, 91, 258, 76, 168),
(37994, 67, 2900, 3000, 0, 136, 92, 262, 77, 171),
(37994, 68, 2953, 3053, 0, 137, 93, 266, 78, 174),
(37994, 69, 3006, 3100, 0, 138, 94, 280, 79, 177),
(37994, 70, 3059, 3212, 0, 139, 95, 284, 80, 180),
(37994, 71, 2489, 4568, 0, 167, 123, 81, 299, 195),
(37994, 72, 2549, 4676, 0, 171, 127, 82, 305, 200),
(37994, 73, 2609, 4784, 0, 175, 131, 83, 311, 205),
(37994, 74, 2669, 4892, 0, 179, 135, 84, 317, 210),
(37994, 75, 2729, 5000, 0, 183, 139, 85, 323, 215),
(37994, 76, 2789, 5108, 0, 187, 143, 86, 329, 220),
(37994, 77, 2849, 5216, 0, 191, 147, 87, 335, 225),
(37994, 78, 2909, 5324, 0, 195, 151, 88, 341, 230),
(37994, 79, 2969, 5432, 0, 199, 155, 89, 347, 235),
(37994, 80, 3029, 5540, 0, 203, 159, 90, 353, 240);

-- item
DELETE FROM item_template WHERE entry IN (50045, 50166);
INSERT INTO `item_template` (`entry`, `class`, `subclass`, `unk0`, `name`, `displayid`, `Quality`, `Flags`, `Faction`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `Duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`) VALUES
(50045, 16, 11, 0, 'Glyph of Eternal Water', 58828, 1, 64, 0, 1, 40000, 100, 0, 128, -1, 55, 50, 0, 0, 0, 0, 0, 0, 0, -1, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70938, 0, -1, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, '', 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, '', 0, 0, 0, 0);

INSERT INTO `item_template` (`entry`, `class`, `subclass`, `unk0`, `name`, `displayid`, `Quality`, `Flags`, `Faction`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `Duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`) VALUES ('50166', '9', '6', '-1', 'Technique: Glyph of Eternal Water', '15274', '2', '64', '0', '1', '40000', '0', '0', '-1', '-1', '50', '0', '773', '250', '0', '0', '0', '0', '0', '-1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1000', '0', '0', '55884', '0', '0', '0', '-1', '0', '-1', '71101', '6', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', 'Teaches you how to inscribe a Glyph of Eternal Water.', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '0', '0', '', '0', '0', '0', '0');

-- vendor
DELETE FROM npc_vendor WHERE item=50166;
INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
('30734', '50166', '0', '0', '0'),
('30735', '50166', '0', '0', '0'),
('28723', '50166', '0', '0', '0');
