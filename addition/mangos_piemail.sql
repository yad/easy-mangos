DELETE FROM `mangos_string` WHERE `entry` = 11005;
INSERT INTO `mangos_string` VALUES (11005, 'Player %s %s eMail: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Игрок %s %s eMail: %s');

INSERT INTO `command` VALUES ('piemail', '4', 'Output email adress for account that include player with $player_name.');