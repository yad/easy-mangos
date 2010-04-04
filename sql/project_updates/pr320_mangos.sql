/*Thrill of the Hunt*/
DELETE FROM spell_chain WHERE spell_id IN (34497, 34498, 34499);
INSERT INTO spell_chain VALUES
(34497, 0, 34497, 1, 0),
(34498, 34497, 34497, 2, 0),
(34499, 34498, 34497, 3, 0);
DELETE FROM spell_proc_event WHERE entry IN (34497, 34498, 34499);
INSERT INTO spell_proc_event VALUES (34497, 0, 0, 399360, 8388609, 33281, 0, 2, 0, 0, 0);

/*Damage Shield*/
INSERT INTO spell_chain VALUES
(58872, 0, 58872, 1, 0),
(58874, 58872, 58872, 2, 0);

DELETE FROM `spell_proc_event` WHERE `entry` in (58872, 58874);
INSERT INTO `spell_proc_event`  VALUES (58872, 0, 0, 0, 0, 0, 0, 67, 0, 0, 0);