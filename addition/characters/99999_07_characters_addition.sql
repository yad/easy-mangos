ALTER TABLE addition_flag_dont_delete CHANGE COLUMN id_0006 id_0007 bit;

ALTER TABLE `pet_spell` ADD `spec` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `active`;
ALTER TABLE `pet_spell` DROP PRIMARY KEY;
ALTER TABLE `pet_spell` ADD PRIMARY KEY ( `guid` , `spell` , `spec` );
