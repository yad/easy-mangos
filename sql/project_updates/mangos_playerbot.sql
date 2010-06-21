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

-- Royaume de l'est

-- Ile de Quel'Danas Lvl 70 Conteste
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 1, 'Ile de QuelDanas QUELDANAS', '12974', '-6891', '6.50', 530, '0', 70, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 2, 'Ile de QuelDanas SUNWELL', '12594', '-6776', '14.6', 530, '0', 70, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 3, 'Ile de QuelDanas TERMAGE', '12891', '-7314', '65.56', 530, '0', 70, 70);
-- Bois des Chants Éternels Lvl 1-10 Horde
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 4, 'Bois des Chants Eternels HAUTSOLEIL', '10365', '-6352', '32.62', 530, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 5, 'Bois des Chants Eternels EPERVIER', '9433', '-6780', '16.61', 530, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 6, 'Bois des Chants Eternels BRISECLEMENTE', '8719', '-6717', '77.16', 530, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 7, 'Bois des Chants Eternels PEREGRIN', '9008', '-7514', '85.42', 530, '67', 1, 10); 
-- Terre Fantômes Lvl 10-20 Horde
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 8, 'Terre Fantomes TRANQUILLIEN', '7646', '-6814', '78.94', 530, '67', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 9, 'Terre Fantomes ZULAMAN', '6848', '-7721', '117.05', 530, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 10, 'Terre Fantomes PASSETHAL', '6390', '-6851', '100.52', 530, '0', 10, 20);
-- Maleterre de l'est Lvl 53-60 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 11, 'Maleterre de l est STRAT1', '3277', '-3280', '142.28', 0, '0', 53, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 12, 'Maleterre de l est STRAT2', '3167', '-4042', '104.59', 0, '0', 53, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 13, 'Maleterre de l est VALTERREUR', '3045', '-2787', '103.53', 0, '0', 53, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 14, 'Maleterre de l est CHAPESPOIR', '2279', '-5279', '82.31', 0, '0', 53, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 15, 'Maleterre de l est CROISECORIN', '2143', '-4616', '73.63', 0, '0', 53, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 16, 'Maleterre de l est ACHERUS', '2354', '-5668', '426.03', 609, '0', 53, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 17, 'Maleterre de l est THONDRORIL', '1926', '-2604', '62.75', 0, '0', 53, 60);
-- Malterre de l'ouest Lvl 51-58 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 18, 'Malterre de l ouest SCHOLO', '1083', '-2539', '59.16', 0, '0', 51, 58);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 19, 'Malterre de l ouest NOROIT', '957', '-1435', '63.12', 0, '469', 51, 58);
-- Clairières de Tirisfal Lvl 1-10 Horde
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 20, 'Clairieres de Tirisfal MONASTERE', '2841', '-692', '139.33', 0, '0', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 21, 'Clairieres de Tirisfal BARRICADE', '1717', '-669', '51.12', 0, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 22, 'Clairieres de Tirisfal BRILL', '2191', '239', '35.12', 0, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 23, 'Clairieres de Tirisfal LEGLAS', '1748', '1649', '118.88', 0, '67', 1, 10);
-- Forêt des Pins argentes Lvl 10-20 Horde
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 24, 'Foret des Pins argentes SEPULCRE', '498', '1595', '125', 0, '67', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 25, 'Foret des Pins argentes OMBRECROC', '-235', '1510', '74.90', 0, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 26, 'Foret des Pins argentes GRISETETE', '-759', '1525', '17.28', 0, '0', 10, 20);
-- Montagnes d'altérac Lvl 30-40 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 27, 'Montagnes d alterac DALARAN', '193', '314', '51.79', 0, '0', 30, 40);
-- Hinterlands Lvl 40-50 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 28, 'Hinterlands NIDAGLE', '272', '-2023', '183.72', 0, '469', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 29, 'Hinterlands VENGEBROCHE', '-565', '-4545', '11.97', 0, '67', 40, 50);
-- Hautebrande Lvl 20-30 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 30, 'Hautebrande TARREN', '-53', '-925', '55.53', 0, '67', 20, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 31, 'Hautebrande AUSTRIVAGE', '-905', '-535', '6.88', 0, '469', 20, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 32, 'Hautebrande PURIFICATION', '-1240.848', '434.384', '3.052', 0, '0', 20, 30);
-- Arathi Lvl 30-40 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 33, 'Arathi THORADIN', '-828', '-1577', '54.17', 0, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 34, 'Arathi ORNIERE', '-1254', '-2524', '20.78', 0, '469', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 35, 'Arathi TREPASORGRIM', '-920', '-3493', '70.46', 0, '67', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 36, 'Arathi FERMEVIDE', '-1850', '-4148', '9.87', 0, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 37, 'Arathi VIADUC', '-2355', '-2503', '88.35', 0, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 38, 'Arathi FALDIR', '-2154', '-1973', '15.01', 0, '0', 30, 40);
-- Les Paluns Lvl 20-30 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 39, 'Les Paluns DUNMODR', '-2656', '-2459', '80.02', 0, '469', 20, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 40, 'Les Paluns GRIMBATOL', '-4071', '-3456', '281.39', 0, '0', 20, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 41, 'Les Paluns MENETHIL', '-3601', '-870', '12.94', 0, '469', 20, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 42, 'Les Paluns VV1 VIDE', '-3976', '-1443', '173.72', 0, '0', 20, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 43, 'Les Paluns EXCAVATIONSTARREL', '-3325.17', '-1862.74', '26.40', 0, '469', 20, 30);
-- Loch Modan Lvl 10-20 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 44, 'Loch Modan BARRFORMEPIERRE', '-4710', '-3137', '310', 0, '469', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 45, 'Loch Modan THELSAMAR', '-5352', '-2951', '323.85', 0, '469', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 46, 'Loch Modan TUNELFORMEPIERRE', '-6099', '-2405', '294.92', 0, '469', 10, 20);
-- Dun Morog Lvl 1-10 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 47, 'Dun Morog GOLBOLAR', '-5700', '-1596', '383.21', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 48, 'Dun Morog FRIGERE', '-6226', '334', '383.21', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 49, 'Dun Morog BRASSETOUT', '-5387', '302', '394.06', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 50, 'Dun Morog GNOMEREGAN', '-5188', '518', '387.83', 0, '0', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 51, 'Dun Morog KHARANOS', '-5638', '-468', '397.69', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 52, 'Dun Morog AEROPORTFF', '-4551', '-1640', '503.33', 0, '0', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 53, 'Dun Morog DEBARCNEMAN', '-6369', '1254', '8.75', 0, '0', 1, 10);
-- Terres ingrates Lvl 35-45 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 54, 'Terres ingrates ULDAMAN', '-6103', '-3304', '256.18', 0, '0', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 55, 'Terres ingrates KARGATH', '-6685', '-2196', '248.75', 0, '67', 35, 45);
-- Gorge des Vents brulants Lvl 43-50 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 56, 'Gorge des Vents brulants THORIUM', '-6532', '-1165', '309.25', 0, '0', 43, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 57, 'Gorge des Vents brulants ROCHENOIRE', '-7326', '-1086', '277.07', 0, '0', 43, 50);
-- Steppes ardentes Lvl 50-58 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 58, 'Steppes ardentes AUTELTEMPETE', '-7626', '-712', '183.25', 0, '0', 50, 58);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 59, 'Steppes ardentes CORNICHEFLAM', '-7516', '-2181', '162.94', 0, '0', 50, 58);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 60, 'Steppes ardentes VEILLEMORGAN', '-8372', '-2753', '186.60', 0, '469', 50, 58);
-- Les Carmines Lvl 15-25 Contesté 
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 61, 'Les Carmines COMTELAC', '-9275', '-2210', '64.06', 0, '469', 15, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 62, 'Les Carmines TROISCHEMINS', '-9620', '-1907', '59.76', 0, '469', 15, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 63, 'Les Carmines GUETDEPIERRE', '-9475', '-3008', '134.45', 0, '0', 15, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 64, 'Les Carmines GALARDEL', '-9263.22', '-3405', '106.58', 0, '0', 15, 25);
-- Forêt d'Elwynn Lvl 1-10 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 65, 'Foret d Elwynn BUCHERONS', '-9467', '-1267', '43', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 66, 'Foret d Elwynn GOLDSHIRE', '-9464', '63', '55.86', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 67, 'Foret d Elwynn NORDSHIRE', '-8943', '-139', '83.64', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 68, 'Foret d Elwynn GARNRUISS', '-9758', '676', '27.13', 0, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 69, 'Foret d Elwynn PSTORWIND', '-8823.7', '796.6', '97.31', 0, '0', 1, 10);
-- Marche de l'ouest Lvl 10-20 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 70, 'Marche de l ouest COLLSENTI', '-10545', '1077', '50.14', 0, '469', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 71, 'Marche de l ouest MORTEMINES', '-11081', '1563', '48.67', 0, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 72, 'Marche de l ouest VIEUPHARE', '-11408.48', '1982.29', '10.74', 0, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 73, 'Marche de l ouest COLDAGUE', '-11266', '1441', '89.52', 0, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 74, 'Marche de l ouest MORTEVEILLE', '-11126.37', '570.06', '35.91', 0, '0', 10, 20);
-- Bois de la pénombre Lvl 18- 30 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 75, 'Bois de la penombre CAMPSVEN', '-10296.6', '640.22', '28.12', 0, '469', 18, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 76, 'Bois de la penombre ABERCROMBIE', '-10284.91', '82.62', '38.84', 0, '469', 18, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 77, 'Bois de la penombre COCORBO', '-10746', '325', '38.26', 0, '469', 18, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 78, 'Bois de la penombre SOMBRECOMTE', '-10569', '-1182', '28.09', 0, '469', 18, 30);
-- Défilé de Deuillevent Lvl 55-60 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 79, 'Defile de Deuillevent KARAZHAN', '-11131', '-2005', '48.15', 0, '0', 55, 60);
-- Strangleronce Lvl 30-45 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 80, 'Strangleronce YOJAMBA', '-11821', '1236', '1.15', 0, '0', 30, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 81, 'Strangleronce CAMPREBEL', '-11324', '-2005', '48.15', 0, '469', 30, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 82, 'Strangleronce ZULGURUB', '-11895', '-866', '34.47', 0, '0', 30, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 83, 'Strangleronce NESINGWARY1', '-11615', '-58', '10.97', 0, '0', 30, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 84, 'Strangleronce GROMOL', '-12388', '171', '2.85', 0, '67', 30, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 85, 'Strangleronce GURUBASHI', '-13225', '233', '33.38', 0, '0', 30, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 86, 'Strangleronce BOUTYBAY', '-14364', '433', '7.36', 0, '0', 30, 45);
-- Terres Foudroyées Lvl 45-55 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 87, 'Terres Foudroyees DARKPORTAL', '-11806', '-3202', '-28.94', 0, '0', 45, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 88, 'Terres Foudroyees REMPARTNEANT', '-10997', '-3410', '61.79', 0, '469', 45, 55);
-- Marais des chagrins Lvl 35-45 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 89, 'Marais des chagrins PIERRECHE', '-10439', '-3528', '20.18', 0, '67', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 90, 'Marais des chagrins TEMPLEENGLOUTI', '-10451', '-3825', '18.07', 0, '0', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 91, 'Marais des chagrins HAVREBOUEUX', '-10112', '-2823', '22.01', 0, '67', 35, 45);
-- Capitale
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 92, 'Capitale LUNEARGENT', '9466', '-7279', '14.21', 530, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 93, 'Capitale UNDERCITY', '1928', '237', '48.05', 0, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 94, 'Capitale IRONFORGE', '-5079', '-792', '495.55', 0, '469', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 95, 'Capitale STORWIND', '-9050.13', '444.79', '93.06', 0, '469', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 96, 'Capitale PORTSTORWIND', '-8420.16', '1176.94', '18.39', 0, '469', 1, 80);

-- Kalimdor

-- Teldrassil Lvl 1-10 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 97, 'Teldrassil SOMBREVALLON', '10322', '829', '1326.38', 1, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 98, 'Teldrassil DOLANAAR', '9852', '967', '1305.73', 1, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 99, 'Teldrassil RUTTHERAN', '8699', '963', '12.5', 1, '469', 1, 10);
-- Sombrivage Lvl 10-20 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 100, 'Sombrivage AUBERDINEP', '6499', '794.63', '7.88', 1, '469', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 101, 'Sombrivage AUBERDINE', '6502', '451.52', '5.98', 1, '469', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 102, 'Sombrivage BONDISSANTE', '7253.719', '-383.481', '28.235', 1, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 103, 'Sombrivage TROLLSFESTIFS', '7433.41', '-1558.39', '187.88', 1, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 104, 'Sombrivage SINUEUSE', '5092.4', '242.07', '28.362', 1, '0', 10, 20);
-- Ile de Brume-azur Lvl 1-10 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 105, 'Ile de Brume-azur AMARVALAAR', '-4254', '-11446', '11.05', 530, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 106, 'Ile de Brume-azur GUETDAZUR', '-4174', '-12517', '44.54', 530, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 107, 'Ile de Brume-azur VALDAMMEN', '-3962.3', '-13930.53', '100.575', 530, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 108, 'Ile de Brume-azur BRUMARGENT', '-5352.34', '-11171.09', '6.923', 530, '469', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 109, 'Ile de Brume-azur ACCOSTAGEODESYUS', '-4695.25', '-12394.18', '11.609', 530, '469', 1, 10);
-- Ile de Brume Sang Lvl 10-20 Alliance
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 110, 'Ile de Brume Sang KESSEL', '-2665.15', '-12319.99', '16.513', 530, '469', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 111, 'Ile de Brume Sang GUETDUSANG', '-2002.5', '-11853.6', '47.41', 530, '469', 10, 20);
-- Reflet de lune  Lvl 1-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 112, 'Reflet de lune REFLETLUNE', '7507.25', '-2198.22', '478.93', 1, '0', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 113, 'Reflet de lune HAVRENUIT', '7889.35', '-2566.25', '487.12', 1, '0', 1, 80);
-- Gangrebois Lvl 48-55 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 114, 'Gangrebois SANCEMEURAUDE', '3982.22', '-1291.22', '248', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 115, 'Gangrebois POSTVENENEUSE', '5120.89', '-350.82', '356.36', 1, '67', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 116, 'Gangrebois GRIFFEBRANCHE', '6806.83', '-1934.33', '569.35', 1, '469', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 117, 'Gangrebois GRUMGUEULE', '6806.83', '-2089.34', '624.83', 1, '0', 48, 55);
-- Berceau de l'hiver Lvl 55-60 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 118, 'Berceau de l hiver SOURCESCHAUDES', '6393.35', '-2531.43', '514.33', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 119, 'Berceau de l hiver PLUIEDETOILES', '7099.76', '-4008.98', '744.82', 1, '469', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 120, 'Berceau de l hiver LONGGUET', '6722.92', '-4654.39', '720.91', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 121, 'Berceau de l hiver MAZTHORIL', '6109.629', '-4189.439', '850.45', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 122, 'Berceau de l hiver TOITDUMONDE', '5129.297', '-3800.593', '1971.06', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 123, 'Berceau de l hiver HYJALFUN', '5475', '-3729', '1594', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 124, 'Berceau de l hiver POINTDECHANGE', '5091.93', '-5095.69', '923.882', 1, '0', 55, 60);
-- Azshara Lvl 45-55 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 125, 'Azshara VALORMOK', '3600.25', '-4412.23', '113.781', 1, '67', 45, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 126, 'Azshara TETEDOURS', '3952.48', '-4746.53', '266.572', 1, '0', 45, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 127, 'Azshara HALTETALRENDIS', '2702.3', '-3885.755', '106.5628', 1, '469', 45, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 128, 'Azshara DUCHYDRAXIS', '2865.391', '-7292.287', '16.651', 1, '0', 45, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 129, 'Azshara HELIPORTAZS', '2253.661', '-7222.115', '14.1519', 1, '0', 45, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 130, 'Azshara TOURDELDARA', '4257.7836', '-7749.1196', '15.151', 1, '0', 45, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 131, 'Azshara CRATEREAZSHARA', '2951.3', '-4785.23', '235.06', 1, '0', 45, 55);
-- Orneval Lvl 18-30 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 132, 'Orneval ZORAMGAR', '3367', '1000', '4.85', 1, '67', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 133, 'Orneval BRASSENOIRE', '4082.75', '822.27', '1.217', 1, '0', 10, 20);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 134, 'Orneval CHANTDESFORETS', '2880.62', '-3157.70', '174.638', 1, '469', 18, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 135, 'Orneval BOISBRISE', '2311.414', '-2534.658', '100.556', 1, '67', 18, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 136, 'Orneval FALFARREN', '1978.238', '-1966.951', '99.1864', 1, '0', 18, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 137, 'Orneval ASTRANAAR', '2722.86', '-384.35', '107.0899', 1, '469', 18, 30);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 138, 'Orneval BOSQUETAILEARGENT', '1464.22', '-1859.28', '122.86', 1, '469', 18, 30);
-- Durotar Lvl 1-10 Horde
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 139, 'Durotar RAGEFEU', '1808.738', '-4396.105', '-18.15', 1, '0', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 140, 'Durotar TRANCHECOLLINE', '313.22', '-4748.56', '9.56', 1, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 141, 'Durotar SENJIN', '-836.452', '-4919.268', '20.23', 1, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 142, 'Durotar FURIEDUSUD', '315.497', '-3759.538', '38.2995', 1, '0', 1, 10);
-- Les Tarides Lvl 10-25 Horde
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 143, 'Les Tarides BRUMEFUNESTE', '344.274', '-2297.776', '246.207', 1, '0', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 144, 'Les Tarides LACROISEE', '-445.77', '-2632.20', '95.461', 1, '67', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 145, 'Les Tarides MORSHAN', '1039.133', '-2147.656', '122.967', 1, '67', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 146, 'Les Tarides LAMENTATION', '-724.282', '-2227.852', '17.0923', 1, '0', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 147, 'Les Tarides TAURAJO', '-2355.75', '-1928.79', '95.8233', 1, '67', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 148, 'Les Tarides CABESTAN', '-955.905', '-3741.738', '5.595', 1, '0', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 149, 'Les Tarides BAELMODAN', '-4132.444', '-2073.328', '85.1166', 1, '469', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 150, 'Les Tarides SOUILLESTRANCHEBAUGE', '-4382.165', '-1944.293', '87.197', 1, '0', 10, 25);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 151, 'Les Tarides KRAALTRANCHEBAUGE', '-4476.859', '-1804.247', '86.292', 1, '0', 10, 25);
-- Mulgore Lvl 1-10 Horde
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 152, 'Mulgore SABOTDESANG', '-2316.91', '-375.91', '-9.1918', 1, '67', 1, 10);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 153, 'Mulgore CAMPNARACHE', '-2924.28', '-270.263', '53.87', 1, '67', 1, 10);
-- Mille pointes Lvl 25-35 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 154, 'Mille pointes GRANDELEVATION', '-4687.58', '-1836.86', '-44.049', 1, '67', 25, 35);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 155, 'Mille pointes BLANCRELAIS', '-4906.05', '-1366.99', '-52.601', 1, '67', 25, 35);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 156, 'Mille pointes LIBREVENT', '-5429.858', '-2416.359', '89.299', 1, '67', 25, 35);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 157, 'Mille pointes AUTELCHAMAN', '-5473.402', '-3146.636', '98.623', 1, '0', 25, 35);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 158, 'Mille pointes CAMPROCHEFER', '-5845.445', '-3422.929', '-50.933', 1, '67', 25, 35);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 159, 'Mille pointes PISTEDESMIRAGES', '-6207.374', '-3866.25', '-58.75', 1, '469', 25, 35);
-- Tanaris Lvl 40-50 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 160, 'Tanaris GADGETZAN', '-7175.949', '-3773.045', '8.375', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 161, 'Tanaris PGENTEPRESSION', '-6940.668', '-4837.9092', '0.669', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 162, 'Tanaris CRIQUEDESGREEMENTS', '-7834.655', '-5053.069', '4.242', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 163, 'Tanaris GROTTETEMPS', '-8194.41', '-4636.134', '8.8775', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 164, 'Tanaris GOUFFREBEANT', '-9066.371', '-3688.881', '40.472', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 165, 'Tanaris ULDUM', '-9593.12', '-2869.62', '32.31', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 166, 'Tanaris CHARDONNIERS', '-8874.74', '-2261.475', '32.23262', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 167, 'Tanaris ZULFARRAK', '-6811.941', '-2888.041', '8.907', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 168, 'Tanaris PILLIERBRISE', '-7987.52', '-3855.62', '17.816', 1, '0', 40, 50);
-- Cratère d'Ungoro Lvl 48-55 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 169, 'Cratere d Ungoro REFUGEMARSHAL', '-6166.7', '-1092.11', '-209.006', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 170, 'Cratere d Ungoro CRISTALNORD', '-6275.307', '-1535.322', '-223.761', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 171, 'Cratere d Ungoro CRETEFOURNAISE', '-7196.18', '-1387.11', '-231.45', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 172, 'Cratere d Ungoro SOURCEGOLAKKA', '-7210.49', '-651.34', '-234.71', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 173, 'Cratere d Ungoro PILONEOUEST', '-7404.28', '-341.83', '-223.173', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 174, 'Cratere d Ungoro PILONEEST', '-7201.166', '-2347.43', '-228.851', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 175, 'Cratere d Ungoro TORWA', '-7840.33', '-2117.32', '-254.269', 1, '0', 48, 55);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 176, 'Cratere d Ungoro BALAFRESINUEUSE', '-7879.295', '-1310.90', '-275.86', 1, '0', 48, 55);
-- Marais d'Aprefange Lvl 35-45 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 177, 'Marais d Aprefange THERAMORE', '-3879.40', '-4595.67', '9.216', 1, '469', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 178, 'Marais d Aprefange NATPAGGLE1', '-4158.371', '-4042.0935', '7.6942', 1, '0', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 179, 'Marais d Aprefange DESERTTHERAM', '-3962.023', '-4960.93', '6.8782', 1, '0', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 180, 'Marais d Aprefange ILEALCAZ', '-2703.823', '-4667.926', '10.164', 1, '0', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 181, 'Marais d Aprefange FLAMEROLES', '-2967.79', '-3871.592', '32.989', 1, '469', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 182, 'Marais d Aprefange MURFOUGERE', '-3133.17', '-2888.996', '34.5964', 1, '67', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 183, 'Marais d Aprefange AUBREPOSOMBRE', '-3701.74', '-2538.78', '69.395', 1, '0', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 184, 'Marais d Aprefange TABETHA', '-4055.501', '-3380.702', '35.031', 1, '469', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 185, 'Marais d Aprefange BOURBABROC', '-4581.178', '-3173.707', '34.171', 1, '0', 35, 45);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 186, 'Marais d Aprefange ONYXIA', '-4754.75', '-3642.192', '46.504', 1, '0', 35, 45);
-- Silithus Lvl 55-60 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 187, 'Silithus REPOSVAILLANTS', '-6380.73', '-308.13', '-1.885', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 188, 'Silithus FORTEARMURE', '-6514.52', '65.93', '156.1565', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 189, 'Silithus FORTCENARIEN', '-6831.68', '758.49', '43.167', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 190, 'Silithus CAMPCREPU', '-6157.31', '1765.27', '28.98', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 191, 'Silithus MURSCARAB', '-8068.58', '1528.81', '2.611', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 192, 'Silithus AQ20', '-8403.45', '1492.26', '23.702', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 193, 'Silithus AQ40', '-8251.77', '1977.41', '129.072', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 194, 'Silithus PLANQUEORTELL', '-7578.72', '196.91', '11.548', 1, '0', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 195, 'Silithus CAMPPVPHORDE', '-7537.45', '751.21', '-17.591', 1, '67', 55, 60);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 196, 'Silithus CAMPPVPALLIANCE', '-7189.1', '1406.78', '3.989', 1, '469', 55, 60);
-- Feralas Lvl 40-50 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 197, 'Feralas THALANAAR', '-4476.21', '-767.186', '-37.615', 1, '469', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 198, 'Feralas ILEEFFROI', '-5332.07', '3732.01', '9.035', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 199, 'Feralas PENNELUNE', '-4434.33', '3280.39', '11.83', 1, '469', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 200, 'Feralas COTEOUBLIE', '-4347.87', '2415.95', '7.941', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 201, 'Feralas COLOSSEJUMO', '-3293.69', '1999.133', '245.674', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 202, 'Feralas HACHETRIPE', '-4799.78', '1317.41', '85.72', 1, '0', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 203, 'Feralas CAMPMOJACHE', '-4362.09', '172.31', '37.87', 1, '67', 40, 50);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 204, 'Feralas PAVILLONLARISS', '-4097.25', '96.46', '76.365', 1, '0', 40, 50);
-- Désolace Lvl 30-40 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 205, 'Desolace VILLGELKIS', '-2240', '2410.83', '54.335', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 206, 'Desolace MANNOROC', '-1734.62', '1916.97', '59.122', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 207, 'Desolace FOUILLEVIS', '-1416.51', '1501.598', '59.466', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 208, 'Desolace VILLMAGRAM', '-1593.52', '1014.23', '90.421', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 209, 'Desolace VILLKOLKAR', '-922.91', '986.90', '89.355', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 210, 'Desolace SARGERON', '-139.34', '894.05', '124.576', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 211, 'Desolace COMBENIJEL', '195.66', '1308.11', '190.3161', 1, '469', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 212, 'Desolace ETHELRETHOR', '-418.97', '2500.25', '120.951', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 213, 'Desolace ILERANAZJAR', '272.61', '3025.107', '5.573', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 214, 'Desolace MARAUDON', '-1375.07', '2795.70', '112.04', 1, '0', 30, 40);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 215, 'Desolace PROIEDELOMBRE', '-1661.09', '3096.35', '30.499', 1, '67', 30, 40);
-- Serres-rocheuses Lvl 15-27 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 216, 'Serres-rocheuses ROCHESOLEIL', '948.55', '946.25', '104.64', 1, '67', 15, 27);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 217, 'Serres-rocheuses SERRESROCHEUSES', '2678.806', '1450.911', '230.829', 1, '469', 15, 27);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 218, 'Serres-rocheuses LACCOMBE', '1605.99', '96.706', '98.566', 1, '0', 15, 27);
-- Capitale
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 219, 'Durotar ORGRIMMAR', '1560.21', '-4417.29', '8.11', 1, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 220, 'Mulgore PITONSTONNERRE', '-1244.24', '79.55', '128.994', 1, '67', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 221, 'Teldrassil DARNASSUS', '9929', '2495', '1317.82', 1, '469', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 222, 'Ile de Brume-azur EXODAR', '-3950', '-11648.45', '-138.66', 530, '469', 1, 80);

-- Outreterre

-- Peninsule des flammes infernales Lvl 58-63 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 223, 'Peninsule des flammes infernales DARKPORTALOT', '-248.968', '962.52', '84.3395', 530, '0', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 224, 'Peninsule des flammes infernales HALTEFRACAS', '268', '1481', '-16.364', 530, '469', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 225, 'Peninsule des flammes infernales HALTEHONNEUR', '509', '1993', '110.77', 530, '469', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 226, 'Peninsule des flammes infernales HALTEXPEDITION', '-665', '1866', '67.282', 530, '469', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 227, 'Peninsule des flammes infernales THRALLMAR', '145', '2738.32', '99.09', 530, '67', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 228, 'Peninsule des flammes infernales BASTIONHONNEUR', '-680.23', '2638.3', '88.94', 530, '469', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 229, 'Peninsule des flammes infernales CITADELLEFLAMM', '-353.484', '3088.83', '22.1032', 530, '0', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 230, 'Peninsule des flammes infernales POSTEBRISECHINE', '-1325.128', '2366.461', '88.955', 530, '67', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 231, 'Peninsule des flammes infernales GUETEPERVIER', '-655.50', '4167.65', '67.784', 530, '67', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 232, 'Peninsule des flammes infernales TELHAMAT', '180.63', '4333.79', '116.406', 530, '469', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 233, 'Peninsule des flammes infernales POSTECENARIEN', '-297.74', '4729.24', '21.976', 530, '0', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 234, 'Peninsule des flammes infernales CAMPBARBELONGUE', '-1002.45', '4295.647', '66.54', 530, '469', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 235, 'Peninsule des flammes infernales EPAVEZEPELIN', '-1087.15', '3002.58', '8.2464', 530, '0', 58, 63);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 236, 'Peninsule des flammes infernales TRONEKILJAEDEN', '875.69', '2535.02', '298.076', 530, '0', 58, 63);
-- Marécage de Zangar Lvl 60-64 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 237, 'Marecage de Zangar REFUGECENARIEN', '-213.52', '5496.94', '21.64', 530, '0', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 238, 'Marecage de Zangar POSTRATMARAIS', '105.95', '5204.93', '21.522', 530, '67', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 239, 'Marecage de Zangar TELREDOR', '269.45', '6067.61', '130.292', 530, '469', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 240, 'Marecage de Zangar GLISSECROC', '753.078', '6865.45', '-69.322', 530, '0', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 241, 'Marecage de Zangar OREBOR', '1003.5', '7365.62', '36.33', 530, '0', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 242, 'Marecage de Zangar SPOREGAR', '217.48', '8544.09', '22.98', 530, '0', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 243, 'Marecage de Zangar ZABRAJIN', '258.15', '7847.22', '23.555', 530, '67', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 244, 'Marecage de Zangar POSTGARDCENARIEN', '-279.33', '8306.8', '19.99', 530, '0', 60, 64);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 245, 'Marecage de Zangar RUINEBOHAMU', '-273.74', '7238.06', '22.83', 530, '0', 60, 64);
-- Les Tranchantes Lvl 65-68 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 246, 'Les Tranchantes SYLVANAAR', '2061.77', '6855.27', '172.933', 530, '469', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 247, 'Les Tranchantes BASTIONSIRETONNER', '2433.58', '6011.30', '152.944', 530, '67', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 248, 'Les Tranchantes POSTTOSHLEY', '1908.74', '5566.63', '262.45', 530, '469', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 249, 'Les Tranchantes PORTEMORT', '2275.63', '5387.68', '144.46', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 250, 'Les Tranchantes CAMPFORGECOLERE', '2917.67', '4916.56', '265.23', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 251, 'Les Tranchantes BOSQUETERNEL', '2967.08', '5507.15', '143.672', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 252, 'Les Tranchantes REPAIREGRUUL', '3550.19', '5175.62', '-5.632', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 253, 'Les Tranchantes CERCLEVIMGOL', '3280.02', '4640.35', '216.529', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 254, 'Les Tranchantes POINTBASHIR', '3866.65', '5978.66', '291.791', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 255, 'Les Tranchantes ARENETRANCHANTE', '2894.58', '5987.77', '1.906', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 256, 'Les Tranchantes CAMPCOUROUX', '2803.36', '6909.66', '365.67', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 257, 'Les Tranchantes CONVENTAILENOIRE', '3226.95', '7167.12', '167.63', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 258, 'Les Tranchantes TOGRILA', '2338.35', '7268.06', '366.36', 530, '0', 65, 68);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 260, 'Les Tranchantes MOKNATHAL', '2190.02', '4772.52', '156.2', 530, '67', 65, 68);
-- Raz-de-neant Lvl 67-70 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 261, 'Raz-de-neant AREA52', '3052', '3663', '142.67', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 262, 'Raz-de-neant FOUDREFLECHE', '4130', '2992', '349', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 263, 'Raz-de-neant POSTEGARDPROTECTO', '4243', '2220', '139', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 264, 'Raz-de-neant COSMOVRILLE', '2999', '1820', '139.533', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 265, 'Raz-de-neant MECHANAR', '2882.345', '1563.43', '248.883', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 266, 'Raz-de-neant LARCATRAZ', '3302.99', '1353.866', '502.287', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 267, 'Raz-de-neant LEBOTANICA', '3394.66', '1497.48', '179.561', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 268, 'Raz-de-neant LEOEIL', '3090.15', '1405.03', '189.521', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 269, 'Raz-de-neant RASSEMETHEREUM', '3961.66', '2300.63', '124.34', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 270, 'Raz-de-neant ECODOMETERREMDIAN', '3355.51', '2862.91', '141.752', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 271, 'Raz-de-neant MANAFORGEARA', '3859.11', '4071.994', '188.042', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 272, 'Raz-de-neant SIEGESOCRETHAR', '4977.47', '3736.89', '215.42', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 273, 'Raz-de-neant ECODOMECHAMPLOINTAIN', '4923.41', '2831.19', '90.08', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 274, 'Raz-de-neant MANAFORGEULTRIS', '3873.67', '1882.54', '262.53', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 275, 'Raz-de-neant MANAFORGEDURO', '2961.51', '2307.93', '161.63', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 276, 'Raz-de-neant MANAFORGECORUU', '2524.69', '2681.53', '129.02', 530, '0', 67, 70); 
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 277, 'Raz-de-neant MANAFORGEBNAAR', '2826.04', '4277.05', '159.25', 530, '0', 67, 70); 
-- Nagrand Lvl 64-67 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 278, 'Nagrand PLATOELEM', '-598.88', '6718.51', '160.4', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 279, 'Nagrand PASSEVENTEJONC', '-1197.7', '6211.33', '60.531', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 280, 'Nagrand SAFARINESINGW', '-1443.53', '6349.82', '37.695', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 281, 'Nagrand ARENENAGRAND', '-2042.77', '6655.33', '13.055', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 282, 'Nagrand GARADAR', '-1310.94', '7268.41', '34.683', 530, '67', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 283, 'Nagrand HALAA', '-1571', '7947', '-22.65', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 284, 'Nagrand POINTAERIS', '-2064.85', '8553.25', '23.894', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 285, 'Nagrand ALTRUIS', '-1487.7', '8760.86', '28.779', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 286, 'Nagrand COLLINECOGNEGUER', '-879.93', '8691', '251.572', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 287, 'Nagrand TELAAR', '-2671.99', '7266.25', '38.883', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 288, 'Nagrand TRONELEMENTS', '-827.635', '6967.87', '34.449', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 289, 'Nagrand ARENEDESANG', '-711.53', '7937.32', '59.54', 530, '0', 64, 67);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 290, 'Nagrand MAISONNAGRANDZANGAR', '-487.36', '7487.82', '179.97', 530, '0', 64, 67);
-- Forêt de Terrokar Lvl 62-65 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 291, 'Foret de Terrokar SHATTRATH', '-1833.86', '5306.86', '-12.4281', 530, '0', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 292, 'Foret de Terrokar LACSILMYR', '-1276.971', '4868.467', '82.8586', 530, '0', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 293, 'Foret de Terrokar FOURRECENARIEN', '-1942.20', '4696.56', '-2.3355', 530, '0', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 294, 'Foret de Terrokar POSTALERIEN', '-2588.41', '3330.07', '0.682', 530, '469', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 295, 'Foret de Terrokar BASTALERIEN', '-2943.15', '3964.06', '-0.51', 530, '469', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 296, 'Foret de Terrokar FORTBRISEPIERRE', '-2617.86', '4392.156', '33.73', 530, '67', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 297, 'Foret de Terrokar AUCHINDOUN', '-3349.66', '4934.845', '-100.04', 530, '0', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 298, 'Foret de Terrokar CAMPSHATARI', '-3748.87', '5396.50', '-3.381', 530, '0', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 299, 'Foret de Terrokar CARAVANREFUG', '-2831.27', '5073.73', '-16.382', 530, '0', 62, 65);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 300, 'Foret de Terrokar SKETTIS', '-3385.25', '3598.86', '276.046', 530, '0', 62, 65);
-- Vallé d'Ombrelune Lvl 67-70 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 301, 'Valle d Ombrelune VILLAGOMBRELUNE', '-3006.7', '2588.86', '76.731', 530, '67', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 302, 'Valle d Ombrelune FORGEMORT', '-3416.01', '2065.17', '79.953', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 303, 'Valle d Ombrelune MAINGULDAN', '-3535.955', '1402.785', '404.869', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 304, 'Valle d Ombrelune FERMONOROK', '-2797.46', '1247.84', '74.59', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 305, 'Valle d Ombrelune AUTELDAMNATION', '-3599.03', '1886.03', '47.241', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 306, 'Valle d Ombrelune BASTIMARHARDI', '-4032.4', '2194.3', '109.963', 530, '469', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 307, 'Valle d Ombrelune SANCTUMETOILE', '-4090.77', '1113.89', '41.684', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 308, 'Valle d Ombrelune CHAMPAILENEANT', '-4149.14', '775.35', '7.023', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 309, 'Valle d Ombrelune AUTELOMBRES', '-4540.165', '1025.84', '9.32', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 310, 'Valle d Ombrelune CAMPGUEULDRAGONS', '-5052.04', '662.59', '89.571', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 311, 'Valle d Ombrelune TEMPLENOIR', '-3559.44', '545.28', '15.57', 530, '0', 67, 70);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 312, 'Valle d Ombrelune AUTELSHATAR', '-3028.83', '810', '-10.32', 530, '0', 67, 70);

-- Norfendre

-- Fjord Hurlant Lvl 68-72 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 313, 'Fjord Hurlant VALGARDE', '596.48', '-5075.82', '5.48', 571, '469', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 314, 'Fjord Hurlant GROT_VALGARDE', '839.02', '-5540.04', '2.361', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 315, 'Fjord Hurlant PONT_VALGARDE', '242.37', '-5229.71', '308.06', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 316, 'Fjord Hurlant EXCAV_BAELGUN', '27.296', '-5955.34', '0.91', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 317, 'Fjord Hurlant ASC_MURAILL', '436.14', '-6069.88', '321.11', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 318, 'Fjord Hurlant ASC_MURAILL2', '949.77', '-5937.18', '283.19', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 319, 'Fjord Hurlant NIFFLEVAR', '796.44', '-5386.29', '191.09', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 320, 'Fjord Hurlant CHUT_YMIRON', '1283.37', '-5126.2', '79.23', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 321, 'Fjord Hurlant CIM_UTGARDE', '1255.41', '-4852.47', '215.62', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 322, 'Fjord Hurlant SOM_UTGARDE', '1178.09', '-4878.39', '408.81', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 323, 'Fjord Hurlant DONJ_UTGARDE', '1224.71', '-4863.49', '41.25', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 324, 'Fjord Hurlant HALEGRINCE', '795.85', '-4346.82', '167.15', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 325, 'Fjord Hurlant NEW_AGAMAND', '433.75', '-4571.94', '245.08', 571, '67', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 326, 'Fjord Hurlant COLL_BOUCLIER', '-214.60', '-5141.27', '312.173', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 327, 'Fjord Hurlant ASC_PONEVENT', '479.80', '-4027.85', '268.47', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 328, 'Fjord Hurlant CAP_FORBAN', '-133.26', '-3571.26', '3.365', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 329, 'Fjord Hurlant KAMAGUA', '758.61', '-2930.04', '7.162', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 330, 'Fjord Hurlant GARDELOUEST', '1417.34', '-3231.89', '161.98', 571, '469', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 331, 'Fjord Hurlant CAMP_APOTHI', '2114.62', '-2974.23', '148.57', 571, '67', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 332, 'Fjord Hurlant GJALERBRON', '2474.20', '-3361.26', '149.94', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 333, 'Fjord Hurlant CAMP_SABOTDHIVER', '2659.86', '-4381.29', '281.99', 571, '67', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 334, 'Fjord Hurlant FORT_HARDIVAR', '2471.81', '-5035.52', '283.13', 571, '469', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 335, 'Fjord Hurlant ACCOST_VENGEA', '1926.74', '-6166.76', '23.79', 571, '67', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 336, 'Fjord Hurlant MORG', '2053.99', '-4080.38', '235.84', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 337, 'Fjord Hurlant GROTTVIDE2', '2318.25', '-5992.57', '1.82', 571, '0', 68, 72);
-- Les Grisonnes Lvl 73-75 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 338, 'Les Grisonnes BAIEKAPITALRISK', '2879.40', '-1920.78', '19.11', 571, '0', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 339, 'Les Grisonnes DUN_ARGOL', '3453.61', '-5266.93', '258.17', 571, '0', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 340, 'Les Grisonnes LUNESANGUIN', '4572.76', '-5458.04', '1.17', 571, '0', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 341, 'Les Grisonnes RUINEDRAKILJIN', '4698.12', '-4858.12', '31.28', 571, '0', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 342, 'Les Grisonnes THOR_MODAN', '4755.05', '-4559.93', '224.40', 571, '0', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 343, 'Les Grisonnes CAMPBRIGADMARCH', '4592.42', '-4228.67', '178.66', 571, '469', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 344, 'Les Grisonnes CAMPONEQWAH', '3873.02', '-4510.64', '214.83', 571, '67', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 345, 'Les Grisonnes ABATTAGCIELBLEU', '4251.14', '-3019.61', '311.2', 571, '0', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 346, 'Les Grisonnes BASTICONQUET', '3247.38', '-2256.37', '114.73', 571, '67', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 347, 'Les Grisonnes GITEAMBREPIN', '3452.47', '-2774.51', '200.19', 571, '469', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 348, 'Les Grisonnes ZEBHALAK', '4322.59', '-1917.75', '192.13', 571, '0', 73, 75);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 349, 'Les Grisonnes DRAKATAL', '4617.18', '-3382.85', '274.56', 571, '0', 73, 75);
-- Zul'Drak Lvl 74-77 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 350, 'Zul Drak DRAKTHARON', '4773.92', '-2036.18', '229.39', 571, '0', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 351, 'Zul Drak AMPHIANGOI', '5822.27', '-3080.95', '300.41', 571, '0', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 352, 'Zul Drak ZIMTORGA', '5777.87', '-3580.69', '386.50', 571, '67', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 353, 'Zul Drak GUNDRAK_1', '6728.61', '-4634.54', '450.52', 571, '0', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 354, 'Zul Drak GUNDRAK_2', '6928.65', '-4442.09', '450.52', 571, '0', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 355, 'Zul Drak DUBRAJIN', '6909.83', '-4105.30', '467.36', 571, '67', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 356, 'Zul Drak GUETDEBENE2', '5218.98', '-1322.40', '241.994', 571, '67', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 357, 'Zul Drak SEJOURDARGENT', '5513.40', '-2659.09', '303.96', 571, '469', 74, 77);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 358, 'Zul Drak BRECHELUMIER', '5167.99', '-2201.47', '236.54', 571, '0', 74, 77);
-- Forêt du Chant de cristal Lvl 74-76 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 359, 'Foret du Chant de cristal QG_SACCAGSOL', '5603.46', '-694.48', '206.62', 571, '67', 74, 76);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 360, 'Foret du Chant de cristal QG_SURPLOMCOURSEV', '5065.25', '-567.26', '219.98', 571, '469', 74, 76);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 361, 'Foret du Chant de cristal TERRASEGNEIR', '5164.17', '-58.74', '347.44', 571, '0', 74, 76);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 362, 'Foret du Chant de cristal SEJOURPOURPRE2', '5738.16', '1026.92', '174.49', 571, '0', 74, 76);
-- Dalaran Lvl 1-80 Contesté Capitale
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 363, 'Dalaran FORTPOURPRE', '5708.12', '520.97', '649.87', 571, '0', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 364, 'Dalaran DALEGOUT1', '5648.62', '858.43', '570.29', 571, '0', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 365, 'Dalaran SALONVIOLET', '5850.22', '831.52', '846.34', 571, '0', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 366, 'Dalaran DALABANQUE', '5967.30', '613.62', '650.63', 571, '0', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 367, 'Dalaran AIREKRASUS', '5822.67', '456.25', '658.74', 571, '0', 1, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 368, 'Dalaran PORTEPOURPRE', '5805.77', '605.83', '655.43', 571, '0', 1, 80);
-- Desolation des Dragons Lvl 71-74 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 369, 'Desolation des Dragons NAXXRAMASS2', '3669.7', '-1278.41', '243.9', 571, '0', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 370, 'Desolation des Dragons DONJ_GARDHIVER', '3778.916', '-776.073', '195.81', 571, '469', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 371, 'Desolation des Dragons VEXEVENIN', '3244.84', '-698.24', '166.94', 571, '67', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 372, 'Desolation des Dragons TEMPLEREPOSVER', '3614.81', '279.92', '52.24', 571, '0', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 373, 'Desolation des Dragons SANCTUAIDRACON', '3147.75', '595.96', '25.56', 571, '0', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 374, 'Desolation des Dragons PORTMOAKI', '2801.31', '918.67', '21.9', 571, '67', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 375, 'Desolation des Dragons CIMESAUMATRE', '2528.51', '1703.39', '126.7', 571, '0', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 376, 'Desolation des Dragons MARODAGMAR', '3848.85', '1539.75', '89.73', 571, '67', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 377, 'Desolation des Dragons REPOSETOILES2', '3509.34', '2005.70', '65.29', 571, '469', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 378, 'Desolation des Dragons AHNKAHET', '3646.75', '2044.80', '1.79', 571, '0', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 379, 'Desolation des Dragons AHNKAHET2', '3697.24', '2148.93', '35.35', 571, '0', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 380, 'Desolation des Dragons BASTIOFORDRAGON', '4586.39', '1401.9', '192.93', 571, '469', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 381, 'Desolation des Dragons AVANGARDKORKRO', '4943.76', '1161.39', '239.57', 571, '67', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 382, 'Desolation des Dragons JARDREPODELUNE', '3108.37', '2433.03', '45.381', 571, '0', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 383, 'Desolation des Dragons REVUJPONEVENT', '3782.74', '2878.15', '90.56', 571, '67', 71, 74);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 384, 'Desolation des Dragons PASSDEVENTFROID', '3553.21', '3049.90', '24.57', 571, '0', 71, 74);
-- Toundra Boreenne Lvl 68-72 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 385, 'Toundra Boreenne TAUNKALE', '3448.65', '4110.79', '15.89', 571, '67', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 386, 'Toundra Boreenne UNUPE', '3012.39', '4088.39', '23.22', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 387, 'Toundra Boreenne BAISOUFLNJORD', '2765.067', '4645.565', '2.558', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 389, 'Toundra Boreenne DONJBRAVOU4', '2234.879', '5141.283', '5.345', 571, '469', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 390, 'Toundra Boreenne ILCOURCINGLA', '1358.42', '5504.19', '5.38', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 391, 'Toundra Boreenne BASTIONCHANTEGUERRE', '2740.79', '6098.32', '76.93', 571, '67', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 392, 'Toundra Boreenne JETEECHANTEGUERRE', '2791.68', '7007.73', '4.987', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 393, 'Toundra Boreenne ESCARPAMBRE', '3609.287', '5945.25', '136.22', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 394, 'Toundra Boreenne PISTEPMELEVI', '4142.18', '5274.51', '24.882', 571, '469', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 395, 'Toundra Boreenne AVPOSTBORGORK', '4482.88', '5714.55', '81.28', 571, '67', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 396, 'Toundra Boreenne FLECHEDESANG', '4071.386', '3507.52', '152.60', 571, '0', 68, 72); 
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 397, 'Toundra Boreenne BOUCLIETRANSITUS', '3592.51', '6673.05', '195.09', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 398, 'Toundra Boreenne LENEXUS', '3873.54', '6984.08', '77.258', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 399, 'Toundra Boreenne LOCULUS', '3879.8', '6984.64', '106.33', 571, '0', 68, 72);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 400, 'Toundra Boreenne LOILETERN', '3878.04', '6979.94', '152.05', 571, '0', 68, 72);
-- Bassin de Sholazar Lvl 76-78 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 401, 'Bassin de Sholazar PASSAGPERDU', '4751.48', '5555.34', '-3.282', 571, '0', 76, 78);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 402, 'Bassin de Sholazar CAMPDEBASENESINGW', '5557.2', '5752.01', '-76.99', 571, '0', 76, 78);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 403, 'Bassin de Sholazar PERCHOIFAISER', '6107.80', '5651.62', '6.02', 571, '0', 76, 78);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 404, 'Bassin de Sholazar AVANPOSTDORIAN', '6471.76', '5079.46', '-62.43', 571, '0', 76, 78);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 405, 'Bassin de Sholazar BASSRIVDULAC', '5481.31', '4758.21', '-197.52', 571, '0', 76, 78);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 406, 'Bassin de Sholazar COLLINEFRENECOEUR', '5254.01', '4501.34', '-85.35', 571, '0', 76, 78);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 407, 'Bassin de Sholazar PILIERSANGDEVIE', '5490.26', '4078.63', '129.24', 571, '0', 76, 78);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 408, 'Bassin de Sholazar SURPLOMBFAISER', '5644.44', '3429.18', '300.85', 571, '0', 76, 78);
-- La Couronne de Glace Lvl 77-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 409, 'La Couronne de Glace BALARGARDE', '7076.46', '4341.67', '871.52', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 410, 'La Couronne de Glace CIMEDELAMORT2', '7446.97', '4186.56', '314.1', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 411, 'La Couronne de Glace PORTDELASSAUT', '7843.22', '5139.26', '0.55', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 412, 'La Couronne de Glace CAVEAUDESOMBRES', '8411.69', '2711.75', '655.11', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 413, 'La Couronne de Glace ENCEINTOURNOIDARGENT', '8471.46', '890.96', '547.3', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 414, 'La Couronne de Glace LAVANGARDARGENT2', '6168.05', '-57.54', '388.12', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 415, 'La Couronne de Glace ACCOSTHROTHGAR', '10063.87', '1080.25', '107.457', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 416, 'La Couronne de Glace LACIMEDECROISES', '6421.39', '454.82', '511.29', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 417, 'La Couronne de Glace LESSALLESGELEES', '5633.5', '2029.6', '798.28', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 418, 'La Couronne de Glace CITADELCOURGLACE', '5822.74', '2086.66', '636.07', 571, '0', 77, 80);
-- Joug-d'hiver Lvl 77-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 419, 'Joug-d hiver JOUGDHIVER3', '5094.38', '2180.78', '365.6', 571, '469', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 420, 'Joug-d hiver LEBOURBIERGLACIAL', '5024.99', '3674.03', '362.69', 571, '67', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 421, 'Joug-d hiver CAVEAUACHARON', '5444.6', '2840.79', '420.43', 571, '0', 77, 80);
-- Les Pics Foudroyes Lvl 77-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 422, 'Les Pics Foudroyes FORTDUGIVRE', '6674.41', '-229.25', '945.48', 571, '469', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 423, 'Les Pics Foudroyes K3', '6122.58', '-1060.29', '402.61', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 424, 'Les Pics Foudroyes BRUNNHILDAR', '6945.10', '-1585.36', '819.55', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 425, 'Les Pics Foudroyes POINMPACTGROM', '7868.06', '-751.35', '1176.43', 571, '67', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 426, 'Les Pics Foudroyes TEMPLETEMPET2', '7432.38', '-533.4', '1896.85', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 427, 'Les Pics Foudroyes ULD_SALLEDEPIERRE', '8921.62', '-997.85', '1039.46', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 428, 'Les Pics Foudroyes ULD_ULDUAR', '9326.48', '-1114.77', '1245.15', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 429, 'Les Pics Foudroyes ULD_SALLEDEFOUDRE', '9183.42', '-1385.18', '1110.22', 571, '0', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 430, 'Les Pics Foudroyes TUNKALO', '7793.01', '-2829.42', '1218.28', 571, '67', 77, 80);
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 431, 'Les Pics Foudroyes DUNNIFFELEM', '7338.82', '-2622.45', '814.89', 571, '0', 77, 80);

-- Special

-- Ile des MJ Lvl 1-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 432, 'Ile des MJ', '16205', '16218', '1.24', 1, '0', 1, 80);
-- --Foret d elwynn Maison vide cascade Lvl 1-80 Alliance 
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 433, 'Foret d elwynn MAISONVIDECASCADE', '-8267.02', '-221.74', '267.95', 0, '469', 1, 80);
-- Quelthalas Lvl 1-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 434, 'Quelthalas', '4256', '-2785', '6.72', 0, '0', 1, 80);
-- Tanaris Lvl 1-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 435, 'Tanaris TERYAISLAND', '-11276.677', '-4693.9', '1.5582', 1, '0', 1, 80);
-- Les Serres Rocheuses Lvl 1-80 Contesté
Insert into `bot_spawns` (`id`,`description`,`x`,`y`,`z`,`map`,`statut`,`lvlmin`,`lvlmax`) values ( 436, 'Les Serres Rocheuses SCIRIEKAPITALRISK', '1951.53', '1530.72', '247.285', 1, '0', 1, 80);