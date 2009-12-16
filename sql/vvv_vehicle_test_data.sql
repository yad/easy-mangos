UPDATE creature_template SET maxhealth = 133525, minhealth = 133525, maxmana = 51360, minmana = 51360 WHERE entry = 28670;
UPDATE creature_template SET maxhealth = 50000, minhealth = 50000 WHERE entry = 28094;
UPDATE creature_template SET maxhealth = 75000, minhealth = 75000 WHERE entry IN (28312,32627);
UPDATE creature_template SET maxhealth = 50000, minhealth = 50000 WHERE entry IN (28319,32629);

UPDATE creature_template SET minlevel = 80, maxlevel = 80 WHERE entry IN (28312,32627,28319,32629,28094,28670);

UPDATE creature_template SET speed = 2, InhabitType = 1 WHERE entry IN (28312,32627,28319,32629,28094,29929,28782);
UPDATE creature_template SET speed = 2.4, InhabitType = 4 WHERE entry IN (28670);
UPDATE creature_template SET mechanic_immune_mask = 652951551 WHERE entry IN (28670,28312,32627,28319,32629,28094,29929,28782);

DELETE FROM npc_spellclick_spells WHERE npc_entry in (28670, 28312, 32629, 28319, 32627, 28094, 29929, 28782);
INSERT INTO npc_spellclick_spells VALUES (28670, 52196, 0, 0, 0, 0);
INSERT INTO npc_spellclick_spells VALUES (28312, 60968, 0, 0, 0, 1);
INSERT INTO npc_spellclick_spells VALUES (32627, 60968, 0, 0, 0, 1);
INSERT INTO npc_spellclick_spells VALUES (28319, 60968, 0, 0, 0, 1);
INSERT INTO npc_spellclick_spells VALUES (32629, 60968, 0, 0, 0, 1);
INSERT INTO npc_spellclick_spells VALUES (28094, 60968, 0, 0, 0, 1);
INSERT INTO npc_spellclick_spells VALUES (29929, 58961, 0, 0, 0, 1);
INSERT INTO npc_spellclick_spells VALUES (28782, 52280, 12687, 1, 12687, 1);

DELETE FROM creature_template_addon WHERE entry in (28670, 28312, 32629, 28319, 32627, 28094, 29929, 28782);
INSERT INTO creature_template_addon VALUES (28670, 0, 50331648, 1, 0, 1024, 156, NULL, '53112 0 53112 1');
INSERT INTO creature_template_addon VALUES (28312, 0, 0, 0, 0, 0, 117, '28319 7', NULL);
INSERT INTO creature_template_addon VALUES (32627, 0, 0, 0, 0, 0, 117, '32629 7', NULL);
INSERT INTO creature_template_addon VALUES (28319, 0, 0, 0, 0, 0, 116, NULL, NULL);
INSERT INTO creature_template_addon VALUES (32629, 0, 0, 0, 0, 0, 116, NULL, NULL);
INSERT INTO creature_template_addon VALUES (28094, 0, 0, 0, 0, 0, 106, NULL, NULL);
INSERT INTO creature_template_addon VALUES (29929, 0, 0, 0, 0, 0, 210, NULL, NULL);
INSERT INTO creature_template_addon VALUES (28782, 0, 0, 0, 0, 0, 200, NULL, NULL);

DELETE FROM vehicle_data WHERE entry in (106, 116, 117, 156, 200, 210);
INSERT INTO vehicle_data VALUES (106, 4, 50896, 0, 50652, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO vehicle_data VALUES (116, 5, 51362, 51421, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO vehicle_data VALUES (117, 4, 50652, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO vehicle_data VALUES (156, 24, 53114, 0, 53110, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO vehicle_data VALUES (200, 30, 52362, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO vehicle_data VALUES (210, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

DELETE FROM vehicle_seat_data WHERE seat in (1643, 1648, 1649, 1650, 1652, 1554, 1556, 1557, 1986, 1987, 2144, 2181, 2182);
INSERT INTO vehicle_seat_data VALUES (1643, 3);
INSERT INTO vehicle_seat_data VALUES (1648, 3);
INSERT INTO vehicle_seat_data VALUES (1649, 2);
INSERT INTO vehicle_seat_data VALUES (1650, 2);
INSERT INTO vehicle_seat_data VALUES (1652, 4);
INSERT INTO vehicle_seat_data VALUES (1554, 3);
INSERT INTO vehicle_seat_data VALUES (1556, 6);
INSERT INTO vehicle_seat_data VALUES (1557, 6);
INSERT INTO vehicle_seat_data VALUES (1986, 3);
INSERT INTO vehicle_seat_data VALUES (1987, 8);
INSERT INTO vehicle_seat_data VALUES (2144, 1);
INSERT INTO vehicle_seat_data VALUES (2181, 1);
INSERT INTO vehicle_seat_data VALUES (2182, 0);

--Some quests
--Argent tournament:
UPDATE creature_template SET speed = '1.5', unit_flags = 8 WHERE entry IN (33844,33845);
DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id IN (33844,33845));
DELETE FROM creature WHERE id IN (33844,33845);
DELETE FROM vehicle_data WHERE entry in (349);
INSERT INTO `vehicle_data` VALUES ('349', '24', '62544', '62575', '62960', '62552', '64077', '62863', '0', '0', '0', '0', '62853');
DELETE FROM vehicle_seat_data WHERE seat in (3129);
INSERT INTO `vehicle_seat_data` VALUES ('3129', '1')
INSERT INTO `npc_spellclick_spells` VALUES ('33842', '63791', '13829', '1', '0', '3');
INSERT INTO `npc_spellclick_spells` VALUES ('33842', '63791', '13839', '1', '0', '3');
INSERT INTO `npc_spellclick_spells` VALUES ('33842', '63791', '13838', '1', '0', '3');
INSERT INTO `npc_spellclick_spells` VALUES ('33843', '63792', '13828', '1', '0', '3');
INSERT INTO `npc_spellclick_spells` VALUES ('33843', '63792', '13837', '1', '0', '3');
INSERT INTO `npc_spellclick_spells` VALUES ('33843', '63792', '13835', '1', '0', '3');
DELETE FROM creature_template_addon WHERE entry IN (33844,33845);
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (33844, 0, 0, 2049, 0, 0, 349, '', '');
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (33845, 0, 0, 2049, 0, 0, 349, '', '');

--Quest Into the Realm of Shadows (12687):
UPDATE creature_template SET faction_A = 2082, faction_H = 2082, unit_flags = 0 WHERE entry = 28782;
DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id = 28782);
DELETE FROM vehicle_data WHERE entry in (135);
INSERT INTO vehicle_data VALUES (135, 12, 52362, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
DELETE FROM vehicle_seat_data WHERE seat in (1871);
INSERT INTO vehicle_seat_data VALUES (1871, 1);
DELETE FROM npc_spellclick_spells WHERE npc_entry in (28782);
INSERT INTO npc_spellclick_spells VALUES (28782, 52349, 12687, 1, 12687, 3);
DELETE FROM creature_template_addon WHERE entry IN (28782);
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (28782, 0, 0, 1, 0, 0, 135, '', '');

--Quest Grand Theft Palomino (12680):
DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id IN (28605,28606,28607));
INSERT INTO spell_script_target VALUES (52264,1,28653);
DELETE FROM vehicle_data WHERE entry in (123);
INSERT INTO vehicle_data VALUES (123, 12, 52264, 52268, 0, 0, 0, 0, 0, 0, 0, 0, 0);
DELETE FROM vehicle_seat_data WHERE seat in (1782);
INSERT INTO vehicle_seat_data VALUES (1782, 1);
DELETE FROM npc_spellclick_spells WHERE npc_entry in (28605,28606,28607);
INSERT INTO npc_spellclick_spells VALUES (28605, 52263, 12680, 1, 12680, 3);
INSERT INTO npc_spellclick_spells VALUES (28606, 52263, 12680, 1, 12680, 3);
INSERT INTO npc_spellclick_spells VALUES (28607, 52263, 12680, 1, 12680, 3);
DELETE FROM creature_template_addon WHERE entry IN (28605,28606,28607);
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (28605, 0, 0, 1, 0, 0, 123, '', '');
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (28606, 0, 0, 1, 0, 0, 123, '', '');
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (28607, 0, 0, 1, 0, 0, 123, '', '');

--Quest Going Bearback
DELETE FROM npc_spellclick_spells WHERE npc_entry in (29598);
INSERT INTO npc_spellclick_spells VALUES (29598, 54908, 12851, 1, 12851, 1);
DELETE FROM creature_template_addon WHERE entry IN (29598);
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (29598, 0, 0, 1, 0, 0, 308, '', '');
DELETE FROM vehicle_data WHERE entry in (308);
INSERT INTO `vehicle_data` VALUES ('308', '24', '54897', '54907', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO vehicle_seat_data VALUES (2699, 3);

--Mamonth
DELETE FROM creature_template_addon WHERE entry in (32633,32640);
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (32633, 0, 0, 1, 0, 0, 312, '', '');
INSERT INTO creature_template_addon (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `vehicle_id`, `passengers`, `auras`) VALUES (32640, 0, 0, 1, 0, 0, 312, '', '');
DELETE FROM vehicle_data WHERE entry in (312);
