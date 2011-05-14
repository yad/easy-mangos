DROP TABLE IF EXISTS `addition_flag_dont_delete`;
CREATE TABLE `addition_flag_dont_delete` (
  `id_0000` bit(1) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Last applied addition update to DB';

LOCK TABLES `addition_flag_dont_delete` WRITE;
/*!40000 ALTER TABLE `addition_flag_dont_delete` DISABLE KEYS */;
INSERT INTO `addition_flag_dont_delete` VALUES
(NULL);
/*!40000 ALTER TABLE `addition_flag_dont_delete` ENABLE KEYS */;
UNLOCK TABLES;

CREATE TABLE `character_battleground_random` (
  `guid` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
