DROP TABLE IF EXISTS `bot_spawns`;
CREATE TABLE `bot_spawns` (
  `id` int(3) unsigned NOT NULL,
  `description` char(50) NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `map` int(3) unsigned NOT NULL,
  `statut` int(3) unsigned NOT NULL,
  `Lvlmin` tinyint(2) unsigned NOT NULL,
  `Lvlmax` tinyint(2) unsigned NOT NULL,
  PRIMARY KEY (`id`)
);

-- Capitale
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 92, 'Capitale LUNEARGENT', '9466', '-7279', '14.21', 530, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 93, 'Capitale UNDERCITY', '1928', '237', '48.05', 0, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 94, 'Capitale IRONFORGE', '-5079', '-792', '495.55', 0, '469', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 95, 'Capitale STORWIND', '-9050.13', '444.79', '93.06', 0, '469', 1, 80);
-- Capitale
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 219, 'Durotar ORGRIMMAR', '1560.21', '-4417.29', '8.11', 1, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 220, 'Mulgore PITONSTONNERRE', '-1244.24', '79.55', '128.994', 1, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 221, 'Teldrassil DARNASSUS', '9929', '2495', '1317.82', 1, '469', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 222, 'Ile de Brume-azur EXODAR', '-3950', '-11648.45', '-138.66', 530, '469', 1, 80);
