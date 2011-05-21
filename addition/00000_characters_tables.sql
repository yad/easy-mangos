-- Random Battleground
-- Commit 0525ca144e282cec2478

DROP TABLE IF EXISTS `character_battleground_random`;
CREATE TABLE `character_battleground_random` (
  `guid` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- BOP item trade

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `item_soulbound_trade_data` (
    `itemGuid` int(16) unsigned NOT NULL DEFAULT '0',
    `allowedPlayers` varchar(255) NOT NULL DEFAULT '',
    PRIMARY KEY (`itemGuid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='BOP item trade cache';

-- Wow Armory

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `armory_character_stats` (
    `guid` int(11) NOT NULL,
    `data` longtext NOT NULL,
    PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='World of Warcraft Armory table';

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `armory_character_feed_log` (
    `guid` int(11) NOT NULL,
    `type` smallint(1) NOT NULL,
    `data` int(11) NOT NULL,
    `date` int(11) default NULL,
    `counter` int(11) NOT NULL,
    `difficulty` smallint(6) default '-1',
    `item_guid` int(11) default '-1',
    PRIMARY KEY (`guid`, `type`, `data`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='World of Warcraft Armory feed log';

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `armory_game_chart` (
    `gameid` int(11) NOT NULL,
    `teamid` int(11) NOT NULL,
    `guid` int(11) NOT NULL,
    `changeType` int(11) NOT NULL,
    `ratingChange` int(11) NOT NULL,
    `teamRating` int(11) NOT NULL,
    `damageDone` int(11) NOT NULL,
    `deaths` int(11) NOT NULL,
    `healingDone` int(11) NOT NULL,
    `damageTaken` int(11) NOT NULL,
    `healingTaken` int(11) NOT NULL,
    `killingBlows` int(11) NOT NULL,
    `mapId` int(11) NOT NULL,
    `start` int(11) NOT NULL,
    `end` int(11) NOT NULL,
    PRIMARY KEY  (`gameid`, `teamid`, `guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='WoWArmory Game Chart';

-- Group flags and roles support
-- Commit a5e57729fc5211bb6a2f

ALTER TABLE `group_member`
    ADD COLUMN  `roles` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member roles bit mask' AFTER `subgroup`;

ALTER TABLE `group_member`
    CHANGE `assistant` `memberFlags` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member flags bit mask';

ALTER TABLE `groups`
    DROP `mainTank`,
    DROP `mainAssistant`;

-- dungeon DBC encounters support

ALTER TABLE `instance`
    ADD COLUMN `encountersMask` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Dungeon encounter bit mask' AFTER `difficulty`;

-- Instance Extend LFG
-- Commit 020d4e346d38b961bf62

ALTER TABLE `character_instance`
    ADD COLUMN `extend` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Instance extend (bool)' AFTER `permanent`;

-- Refer a friend

ALTER TABLE `characters`
    ADD COLUMN `grantableLevels` tinyint(3) unsigned NOT NULL default '0' AFTER `actionBars`;

-- Saved Variables
-- Commit 0525ca144e282cec2478

ALTER TABLE `saved_variables`
    ADD COLUMN `NextRandomBGResetTime` bigint(40) unsigned NOT NULL default 0 AFTER `NextWeeklyQuestResetTime`;
