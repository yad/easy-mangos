DELETE FROM spell_chain WHERE spell_id IN (3674, 63668, 63669, 63670, 63671, 63672 );
INSERT INTO spell_chain VALUES
(3674, 0, 3674, 1, 0),
(63668, 3674, 3674, 2, 0),
(63669, 63668, 3674, 3, 0),
(63670, 63669, 3674, 4, 0),
(63671, 63670, 3674, 5, 0),
(63672, 63671, 3674, 6, 0);

DELETE FROM spell_bonus_data WHERE entry = 3674;
INSERT INTO spell_bonus_data VALUES
(3674, 0, 1, 0.02, "Hunter - Black Arrow");

DELETE FROM spell_chain WHERE spell_id IN (13812, 14314, 14315, 27026, 49064, 49065);
INSERT INTO spell_chain VALUES
(13812, 0, 13812, 1, 0),
(14314, 13812, 13812, 2, 0),
(14315, 14314, 13812, 3, 0),
(27026, 14315, 13812, 4, 0),
(49064, 27026, 13812, 5, 0),
(49065, 49064, 13812, 6, 0);

DELETE FROM spell_bonus_data WHERE entry = 13812;
INSERT INTO spell_bonus_data VALUES
(13812, 1, 1.6, 0.1, "Hunter - Explosive Trap Effect");

DELETE FROM `spell_chain` WHERE spell_id IN (13797, 14298, 14299, 14300, 14301, 27024, 49053, 49054);
INSERT INTO `spell_chain` VALUES
('14299','14298','13797','3','0'),
('14298','13797','13797','2','0'),
('13797','0','13797','1','0'),
('14300','14299','13797','4','0'),
('14301','14300','13797','5','0'),
('27024','14301','13797','6','0'),
('49053','27024','13797','7','0'),
('49054','49053','13797','8','0');

DELETE FROM spell_bonus_data WHERE entry = 13797;
INSERT INTO spell_bonus_data VALUES
(13797, 0, 0, 0, "Hunter - immolation trap Effect (magic = no RAP)");