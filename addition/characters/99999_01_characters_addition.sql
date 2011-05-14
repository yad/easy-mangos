ALTER TABLE addition_flag_dont_delete CHANGE COLUMN id_0000 id_0001 bit;

ALTER TABLE `saved_variables` ADD COLUMN `NextRandomBGResetTime` bigint(40) unsigned NOT NULL default '0' AFTER `NextWeeklyQuestResetTime`;
