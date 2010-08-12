DELETE FROM `command` WHERE `name` IN ('npc clonemodel', 'npc clonemodel id');
INSERT INTO `command` VALUES ('npc clonemodel',    '3', 'Set your model to model of selected creature');
INSERT INTO `command` VALUES ('npc clonemodel id', '3', 'Set your model to model of creature with entered ID');