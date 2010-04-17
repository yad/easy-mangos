UPDATE creature_template SET modelid_A = 16925, modelid_H = 16925, scale = 2, 
unit_flags = 33554432, flags_extra = 0 WHERE entry IN (28525, 28542, 28543, 28544);

UPDATE creature_template SET ScriptName = 'npc_eye_of_acherus' WHERE entry = 28511;
UPDATE gameobject_template SET ScriptName = 'go_eye_of_acherus' WHERE entry = 191609;
UPDATE creature SET PhaseMask = 2 WHERE id IN (28525, 28542, 28543, 28544);

DELETE FROM spell_script_target WHERE entry IN (51858, 51859);
INSERT INTO spell_script_target (`entry`, `type`, `targetEntry`) VALUES 
(51858,1,28525),
(51858,1,28542),
(51858,1,28543),
(51858,1,28544),
(51859,1,28525),
(51859,1,28542),
(51859,1,28543),
(51859,1,28544);

DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id IN 
(28525, 28542, 28543, 28544, 28511));

DELETE FROM creature_template_addon WHERE entry IN (28525, 28542, 28543, 28544, 28511);
INSERT INTO creature_template_addon (`entry`, `moveflags`) VALUES (28511, 33562624);
INSERT INTO creature_template_addon (`entry`, `auras`) VALUES 
(28525,'64328 0'),
(28542,'64328 0'),
(28543,'64328 0'),
(28544,'64328 0');

DELETE FROM `gameobject_template` WHERE (`entry`=191609);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES (191609, 10, 8123, 'Eye of Acherus Control Mechanism', '', '', '', 0, 6553636, 1, 0, 0, 0, 0, 0, 0, 1635, 12641, 0, 0, 0, 0, 0, 0, 0, 0,     0     , 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'go_eye_of_acherus');
