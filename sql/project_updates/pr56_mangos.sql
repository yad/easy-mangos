-- Jeeves
DELETE FROM `creature_template` WHERE entry = 35642;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid_A`, `modelid_A2`, `modelid_H`, `modelid_H2`, `name`, `subname`, `IconName`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `unk16`, `unk17`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES('35642','0','0','0','0','0','30076','0','30076','0','Jeeves','','','80','80','10000','10000','0','0','0','35','35','135297','1','1','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','3','1','0','0','0','0','0','0','0','0','1','0','0','0','');

DELETE FROM `npc_vendor` WHERE (`entry`=35642);
INSERT INTO `npc_vendor` (entry, item, maxcount, incrtime, ExtendedCost) VALUES 
(35642, 41584, 0, 0, 0),
(35642, 41586, 0, 0, 0),
(35642, 44605, 0, 0, 0),
(35642, 44614, 0, 0, 0),
(35642, 44615, 0, 0, 0),
(35642, 16583, 0, 0, 0),
(35642, 21177, 0, 0, 0),
(35642, 17020, 0, 0, 0),
(35642, 37201, 0, 0, 0),
(35642, 5565, 0, 0, 0),
(35642, 17032, 0, 0, 0),
(35642, 17030, 0, 0, 0),
(35642, 17033, 0, 0, 0),
(35642, 17031, 0, 0, 0);

DELETE FROM item_template WHERE entry=49050;
INSERT INTO `item_template` (`entry`, `class`, `subclass`, `unk0`, `name`, `displayid`, `Quality`, `Flags`, `Faction`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `Duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`) VALUES('49050','9','3','-1','Schematic: Jeeves','6270','4','64','0','0','0','0','0','-1','-1','80','0','202','450','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','483','0','-1','0','-1','0','-1','68067','6','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','Teaches you how to make Jeeves.','0','0','0','0','0','4','0','0','0','0','0','0','0','0','128','0','0','0','0','0','0','0','0','0','-1','0','0','0','0','','0','0','0','0');

DELETE FROM creature_loot_template WHERE item = 49050;
INSERT INTO creature_loot_template VALUES 
(29724, 49050, 0.5, 0, 1, 1, 7, 202, 1),
(25753, 49050, 0.5, 0, 1, 1, 7, 202, 1),
(27971, 49050, 1, 0, 1, 1, 7, 202, 1),
(27972, 49050, 1, 0, 1, 1, 7, 202, 1),
(27641, 49050, 1, 0, 1, 1, 7, 202, 1),
(28835, 49050, 1, 0, 1, 1, 7, 202, 1);

-- wormhole
UPDATE creature_template SET minhealth = 1, maxhealth = 1, npcflag = 1, ScriptName = 'npc_wormhole' WHERE entry = 35646;

-- Scrapbot
DELETE FROM `creature_template` WHERE (`entry`='29561');
INSERT INTO `creature_template`(`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid_A`, `modelid_A2`, `modelid_H`, `modelid_H2`, `name`, `subname`, `IconName`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `unk16`, `unk17`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES ('29561', '0', '0', '0', '0', '0', '26341', '0', '26341', '0', 'Scrapbot', '', '', '80', '80', '20000', '20000', '0', '0', '0', '35', '35', '4225', '1', '1', '0', '0', '0', '0', '0', '1', '2000', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '9', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '0', '3', '3', '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '');

DELETE FROM npc_vendor WHERE entry = 29561;
INSERT INTO `npc_vendor` VALUES
('29561', '41584', '0', '0', '0'),
('29561', '44605', '0', '0', '0'),
('29561', '44614', '0', '0', '0'),
('29561', '44615', '0', '0', '0'),
('29561', '16583', '0', '0', '0'),
('29561', '21177', '0', '0', '0'),
('29561', '17020', '0', '0', '0'),
('29561', '37201', '0', '0', '0'),
('29561', '5565', '0', '0', '0'),
('29561', '17032', '0', '0', '0'),
('29561', '17030', '0', '0', '0'),
('29561', '17033', '0', '0', '0'),
('29561', '17031', '0', '0', '0');