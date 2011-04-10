ALTER TABLE addition_flag_dont_delete CHANGE COLUMN id_0003 id_0004 bit;

ALTER TABLE `group_member` ADD COLUMN `roles` 
TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member roles bit mask'
AFTER `subgroup`;
ALTER TABLE `group_member` CHANGE `assistant` `memberFlags` 
TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member flags bit mask';

ALTER TABLE `groups` DROP `mainTank`, DROP `mainAssistant`;
