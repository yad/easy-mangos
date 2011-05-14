ALTER TABLE addition_flag_dont_delete CHANGE COLUMN id_0004 id_0005 bit;

ALTER TABLE `instance` ADD COLUMN `encountersMask` 
INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Dungeon encounter bit mask'
AFTER `difficulty`;