ALTER TABLE addition_flag_dont_delete CHANGE COLUMN id_0002 id_0003 bit;

ALTER TABLE `character_instance` ADD COLUMN `extend` 
TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Instance extend (bool)'
AFTER `permanent`;