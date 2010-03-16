ALTER TABLE `battleground_events` CHANGE `event1` `event1` SMALLINT( 5 )
UNSIGNED NOT NULL ,
CHANGE `event2` `event2` SMALLINT( 5 ) UNSIGNED NOT NULL;

ALTER TABLE `creature_battleground` CHANGE `event1` `event1` SMALLINT( 5 )
UNSIGNED NOT NULL ,
CHANGE `event2` `event2` SMALLINT( 5 ) UNSIGNED NOT NULL;

ALTER TABLE `gameobject_battleground` CHANGE `event1` `event1` SMALLINT( 5 )
UNSIGNED NOT NULL ,
CHANGE `event2` `event2` SMALLINT( 5 ) UNSIGNED NOT NULL;


-- drop table creature_map;
-- drop table gameobject_map;
-- drop table map_events;
RENAME TABLE creature_battleground TO creature_mapevent;
RENAME TABLE gameobject_battleground TO gameobject_mapevent;
RENAME TABLE battleground_events TO map_events;

-- doorevents changed
UPDATE creature_mapevent SET event1 = 65534 WHERE event1= 254;
UPDATE gameobject_mapevent SET event1 = 65534 WHERE event1= 254;
UPDATE map_events SET event1 = 65534 WHERE event1= 254;
