ALTER TABLE quest_template
  ADD COLUMN    RewArenaPoints           smallint(5) unsigned NOT NULL default '0' AFTER RewHonorableKills;