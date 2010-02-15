--
-- Table structure for table `vehicle_data`
--

DROP TABLE IF EXISTS `vehicle_data`;
CREATE TABLE `vehicle_data` (
  `entry` mediumint(5) unsigned NOT NULL,
  `flags` mediumint(8) unsigned NOT NULL default '0',
  `Spell1` mediumint(8) unsigned NOT NULL default '0',
  `Spell2` mediumint(8) unsigned NOT NULL default '0',
  `Spell3` mediumint(8) unsigned NOT NULL default '0',
  `Spell4` mediumint(8) unsigned NOT NULL default '0',
  `Spell5` mediumint(8) unsigned NOT NULL default '0',
  `Spell6` mediumint(8) unsigned NOT NULL default '0',
  `Spell7` mediumint(8) unsigned NOT NULL default '0',
  `Spell8` mediumint(8) unsigned NOT NULL default '0',
  `Spell9` mediumint(8) unsigned NOT NULL default '0',
  `Spell10` mediumint(8) unsigned NOT NULL default '0',
  `req_aura` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Vehicle System';