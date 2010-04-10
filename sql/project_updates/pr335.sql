USE `characters`;

DROP TABLE IF EXISTS `character_battleground_status`;

CREATE TABLE `character_battleground_status` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier'
  `daily_bg` BIGINT(20) NOT NULL default '0'
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

USE mangos;
DELETE FROM quest_template WHERE entry IN (11335, 11336, 11337, 11338, 11339, 11340, 11341, 11342, 13405, 13407, 14163, 14164);
DELETE FROM creature_involvedrelation WHERE quest IN (11335, 11336, 11337, 11338, 11339, 11340, 11341, 11342, 13405, 13407, 14163, 14164);
DELETE FROM creature_questrelation WHERE quest IN (11335, 11336, 11337, 11338, 11339, 11340, 11341, 11342, 13405, 13407, 14163, 14164);
DELETE FROM game_event WHERE entry IN (41, 42, 43, 44, 45, 46); 
DELETE FROM game_event_creature_quest WHERE event IN (41, 42, 43, 44, 45, 46);