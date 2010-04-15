/*Lock and Load*/
DELETE FROM `spell_proc_event` WHERE entry IN (56342, 56343, 56344);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(56342, 0, 9, 0x0C, 0x8000000, 0x60000, 0, 0, 0, 0, 0);
DELETE FROM `spell_chain` WHERE spell_id IN (56342, 56343, 56344);
INSERT INTO `spell_chain` (`spell_id`, `prev_spell`, `first_spell`, `rank`, `req_spell`) VALUES
(56342, 0, 56342, 1, 0),
(56343, 56342, 56342, 2, 0),
(56344, 56343, 56342, 3, 0);
/*Entrapment*/
DELETE FROM `spell_proc_event` WHERE entry IN (19184, 19387, 19388);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(19184, 0, 9, 0, 0x2000, 0x40000, 0, 0, 0, 0, 0);