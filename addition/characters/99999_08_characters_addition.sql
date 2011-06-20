ALTER TABLE addition_flag_dont_delete CHANGE COLUMN id_0007 id_0008 bit;

ALTER TABLE `character_pet`
  DROP `resettalents_cost`,
  DROP `resettalents_time`;