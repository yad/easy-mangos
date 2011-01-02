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

INSERT INTO `bot_spawns` VALUES ('1', 'Lune-d\'argent (Capitale - Tente)', '9705.76', '-7261.99', '16.38', '530', '67', '1', '80');
INSERT INTO `bot_spawns` VALUES ('2', 'Fossoyeuse (Capitale - Auberge)', '1645.28', '218.86', '-43.1', '0', '67', '1', '80');
INSERT INTO `bot_spawns` VALUES ('3', 'Orgrimmar (Capitale - Maison)', '1714.96', '-4205.61', '51.88', '1', '67', '1', '80');
INSERT INTO `bot_spawns` VALUES ('4', 'Les Pitons du Tonnerre (Capitale - Tente)', '-1224.44', '154.08', '133.23', '1', '67', '1', '80');
INSERT INTO `bot_spawns` VALUES ('5', 'Forgefer (Capitale - Maison)', '-4811', '-1013.28', '502.47', '0', '469', '1', '80');
INSERT INTO `bot_spawns` VALUES ('6', 'Hurlevent (Capitale - Auberge)', '-8865.51', '676.67', '97.9', '0', '469', '1', '80');
INSERT INTO `bot_spawns` VALUES ('7', 'Darnassus (Capitale - Auberge)', '10139.7', '2215.16', '1329.98', '1', '469', '1', '80');
INSERT INTO `bot_spawns` VALUES ('8', 'L\'Exodar (Capitale - Maison)', '-4279.28', '-11781.7', '-132.16', '530', '469', '1', '80');
INSERT INTO `bot_spawns` VALUES ('9', 'Shattrath (Sanctuaire - Horde)', '-2032.28', '5358.52', '-38.71', '530', '67', '60', '80');
INSERT INTO `bot_spawns` VALUES ('10', 'Shattrath (Sanctuaire - Alliance)', '-1603.02', '5306.75', '-38.57', '530', '469', '70', '80');
INSERT INTO `bot_spawns` VALUES ('11', 'Dalaran (Sanctuaire - Horde)', '5884.66', '486.76', '641.55', '571', '67', '70', '80');
INSERT INTO `bot_spawns` VALUES ('12', 'Dalaran (Sanctuaire - Alliance)', '5673.77', '723.74', '642.11', '571', '469', '70', '80');
