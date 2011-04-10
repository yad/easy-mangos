ALTER TABLE addition_flag_dont_delete CHANGE COLUMN id_0001 id_0002 bit;

CREATE TABLE `item_soulbound_trade_data` (
  `itemGuid` int(16) unsigned NOT NULL DEFAULT '0',
  `allowedPlayers` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`itemGuid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='BOP item trade cache';
