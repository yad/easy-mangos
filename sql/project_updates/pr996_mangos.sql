DELETE FROM `creature_template_addon` WHERE `entry` IN (31897, 31896, 31895, 31894, 31893, 31883);
INSERT INTO `creature_template_addon` (`entry`,`auras`) VALUES 
('31897','59907 0'),
('31896','59907 0'),
('31895','59907 0'),
('31894','59907 0'),
('31893','59907 0'),
('31883','59907 0');

UPDATE `creature_template` SET `unit_flags` = 4, `flags_extra` = 2 WHERE `entry` IN (31897, 31896, 31895, 31894, 31893, 31883);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (31897, 31896, 31895, 31894, 31893, 31883);
INSERT INTO `npc_spellclick_spells` (`npc_entry`,`spell_id`,`quest_start`,`cast_flags`) VALUES 
('31897','60123','0','2'),
('31896','60123','0','2'),
('31895','60123','0','2'),
('31894','60123','0','2'),
('31893','60123','0','2'),
('31883','60123','0','2');

DELETE FROM `spell_proc_event` WHERE entry in (59907);
INSERT INTO `spell_proc_event` VALUES
(59907, 0x00,  0x06, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00004000, 0x00004000, 0x00000000, 0x00000000, 0.000000, 0.000000,  0);

DELETE FROM `spell_proc_event` WHERE entry in (54845);
INSERT INTO `spell_proc_event` VALUES
(54845, 0x00,  0x07, 0x00000004, 0x00000004, 0x00000004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0.000000, 0.000000,  0);

DELETE FROM `spell_proc_event` WHERE entry in (12654);
INSERT INTO `spell_proc_event` VALUES
(12654, 0x00,  0x03, 0x08000000, 0x08000000, 0x08000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0.000000, 100.000000,  0);

DELETE FROM `spell_proc_event` WHERE `entry` IN ('71519', '71562');
INSERT INTO `spell_proc_event` VALUES ('71562', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '60');
INSERT INTO `spell_proc_event` VALUES ('71519', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '60');

DELETE FROM `spell_proc_event` WHERE `entry` IN ('70723', '70808');
INSERT INTO `spell_proc_event` VALUES ('70723',	'72', '7', '5',	'5', '5', '0', '0',	'0', '0', '0', '0', '65536', '2', '0', '0', '0');
INSERT INTO `spell_proc_event` VALUES ('70808',	'8', '11', '400', '400', '400', '0', '0', '0', '0', '0', '0', '0', '2', '0', '0', '0');

/* Item - Paladin T10 Holy 4P Bonus */
DELETE FROM `spell_proc_event` WHERE `entry` = 70756;
INSERT INTO `spell_proc_event` VALUES ('70756', '2', '10', '0x00200000', '0x00200000', '0x00200000', '0', '0', '0', '0', '0', '0', '16384', '0', '0', '0', '0');

/* Item - Priest T10 Healer 2P Bonus */
DELETE FROM `spell_proc_event` WHERE `entry` = 70770;
INSERT INTO `spell_proc_event` VALUES ('70770', '2', '6', '0x00000800', '0x00000800', '0x00000800', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');

/* Item - Shaman T10 Enhancement 2P Bonus */
DELETE FROM `spell_proc_event` WHERE `entry` = 70830;
INSERT INTO `spell_proc_event` VALUES ('70830', '1', '11', '0', '0', '0', '0x00020000', '0x00020000', '0x00020000', '0', '0', '0', '16384', '0', '0', '0', '0');

/* Item - Icecrown 25 Normal & Heroic Dagger Proc */
DELETE FROM `spell_proc_event` WHERE `entry` IN ('71880', '71892');
INSERT INTO `spell_proc_event` VALUES ('71880', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '20', '0');
INSERT INTO `spell_proc_event` VALUES ('71892', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '20', '0');

/* Item - Paladin T10 Holy 2P Bonus */
DELETE FROM `spell_proc_event` WHERE `entry` = 70755;
INSERT INTO `spell_proc_event` VALUES ('70755', '2', '10', '0', '0', '0', '0x80004000', '0x80004000', '0x80004000', '0', '0', '0', '16384', '0', '0', '0', '0');

DELETE FROM `spell_proc_event` WHERE entry in (49194);
INSERT INTO `spell_proc_event` VALUES
(49194, 0x00,  0x0F, 0x00002000, 0x00002000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000,  0);

DELETE FROM `spell_bonus_data` WHERE `entry` = 50536;
INSERT INTO `spell_bonus_data` VALUES
(50536,0,0,0,'Death Knight - Unholy Blight Triggered');