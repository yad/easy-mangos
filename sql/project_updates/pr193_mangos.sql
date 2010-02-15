DELETE FROM `spell_proc_event` WHERE `entry` IN (49028);
INSERT INTO `spell_proc_event` VALUES
(49028, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00010010, 0x00010000, 0.000000, 0.000000, 0);
update creature_template set `modelid_A` = '11686', ScriptName='npc_runeblade' where entry=27893;