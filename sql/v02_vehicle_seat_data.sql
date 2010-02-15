--
-- Table structure for table `vehicle_seat_data`
--

DROP TABLE IF EXISTS `vehicle_seat_data`;
CREATE TABLE `vehicle_seat_data` (
  `seat` mediumint(5) unsigned NOT NULL,
  `flags` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`seat`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Vehicle Seat System';