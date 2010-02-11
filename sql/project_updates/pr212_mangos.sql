insert into `game_event` (`entry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`) values('41','2010-01-03 06:00:00','2020-01-03 09:00:00','1440','1440','0','Daily BG: Arathi Basin!');
insert into `game_event` (`entry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`) values('42','2010-01-03 06:00:00','2020-01-03 09:00:00','5184000','1440','0','Daily BG: Eye Of Storm!');
insert into `game_event` (`entry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`) values('43','2010-01-03 06:00:00','2020-01-03 09:00:00','5184000','1440','0','Daily BG: Warsong Gulch!');
DELETE FROM creature_questrelation WHERE quest IN (11335, 11336, 11337, 11338, 11339, 11340, 11341, 11342, 13407, 13476, 14164, 13405, 13478, 14163, 11336, 11340);
UPDATE quest_template SET ExclusiveGroup = 11335, RewHonorAddition  = 20, RewArenaPoints = 25 WHERE entry IN (11335, 11336, 11337, 11338, 13405, 14163);
UPDATE quest_template SET ExclusiveGroup = 11339, RewHonorAddition  = 20, RewArenaPoints = 25 WHERE entry IN (11339, 11340, 11341, 11342, 13407, 14164);
INSERT INTO game_event_creature_quest VALUES
(15351, 11335, 41),
(15351, 11337, 42),
(15351, 11338, 43),
(15350, 11339, 41),
(15350, 11341, 42),
(15350, 11342, 43);