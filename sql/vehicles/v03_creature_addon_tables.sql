ALTER TABLE creature_addon
  ADD COLUMN    vehicle_id           smallint(5) unsigned NOT NULL default '0' AFTER moveflags,
  ADD COLUMN    passengers           text AFTER vehicle_id;

ALTER TABLE creature_template_addon
  ADD COLUMN    vehicle_id           smallint(5) unsigned NOT NULL default '0' AFTER moveflags,
  ADD COLUMN    passengers           text AFTER vehicle_id;