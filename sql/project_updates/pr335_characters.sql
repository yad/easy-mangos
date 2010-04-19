
DROP TABLE IF EXISTS `character_battleground_status`;

CREATE TABLE `character_battleground_status` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `daily_bg` BIGINT(20) NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;