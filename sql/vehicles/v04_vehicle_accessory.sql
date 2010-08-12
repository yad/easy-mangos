ALTER TABLE creature_template
  ADD COLUMN `VehicleEntry` mediumint(8) unsigned NOT NULL default '0' AFTER `PetSpellDataId`;

-- Mechano-hog, Mekgineer's Chopper
UPDATE creature_template SET VehicleEntry = 318, IconName = 'vehichleCursor' WHERE entry IN (29929, 32286);

-- Traveler's Tundra Mammoth, Grand Ice Mammoth, Grand Black War Mammoth, Grand Caravan Mammoth
UPDATE creature_template SET VehicleEntry = 312, IconName = 'vehichleCursor' WHERE entry IN (32633, 32640, 31857, 31858, 31861, 31862, 32212, 32213);

-- X-53 Touring Rocket
UPDATE creature_template SET VehicleEntry = 774, IconName = 'vehichleCursor' WHERE entry = 40725;

UPDATE creature_template_addon SET passengers = '33167 1' WHERE entry = 33109;
UPDATE creature_template_addon SET passengers = '33067 7' WHERE entry = 33060;
UPDATE creature_template_addon SET passengers = '33218 1' WHERE entry = 33214;
UPDATE creature_template_addon SET passengers = '33114 0 33114 1 33114 2 33114 3 33139 7' WHERE entry = 33113;
REPLACE INTO creature_template_addon (`entry`,`passengers`) values (27850,'27905 1');
REPLACE INTO creature_template_addon (`entry`,`passengers`) values (32930,'32933 1 32934 7');
REPLACE INTO creature_template_addon (`entry`,`passengers`) values (33114,'33142 1 33143 2');
