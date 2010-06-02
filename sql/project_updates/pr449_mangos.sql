
/*Table structure for table `item_extended_cost` */

DROP TABLE IF EXISTS `item_extended_cost`;

CREATE TABLE `item_extended_cost` (
  `entry` mediumint(16) unsigned NOT NULL COMMENT 'Id of extendedcost',
  `honor` mediumint(16) unsigned NOT NULL COMMENT 'Honor cost',
  `arena_points` mediumint(16) unsigned NOT NULL COMMENT 'Arena points cost',
  `bracket` mediumint(16) unsigned NOT NULL COMMENT 'Minimum bracket, 0 = 2v2, 1 = 3v3, 2 = 5v5, 3 = only 2v2, 4 = only 3v3, 5 = only 5v5',
  `rating` mediumint(16) unsigned NOT NULL COMMENT 'Personal rating needed'
) ENGINE=InnoDB DEFAULT CHARSET=latin1 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;


