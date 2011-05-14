CREATE TABLE `bot_flag_dont_delete` (
  `id` int(3) unsigned NOT NULL,
  PRIMARY KEY (`id`)
);

ALTER TABLE `characters` CHANGE `guid` `guid` int(11) unsigned NOT NULL COMMENT 'Global Unique Identifier' AUTO_INCREMENT;

DELETE FROM `characters` WHERE `account` = 1;

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lerramas', '4', '5', '0', '1', '84214022', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Erelitram', '4', '11', '0', '1', '100732168', '0', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cerrali', '3', '5', '0', '1', '117573888', '1', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zauch', '4', '4', '0', '1', '197634', '1', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Yeakon', '1', '8', '0', '1', '50401033', '7', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dralidia', '8', '8', '1', '1', '151127041', '10', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Astiewen', '2', '7', '1', '1', '117507588', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Abardowien', '5', '5', '1', '1', '50724865', '8', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Erardoth', '2', '9', '0', '1', '33883656', '3', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Unogan', '6', '6', '0', '1', '17236993', '1', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adwiria', '4', '5', '1', '1', '117769219', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Arelan', '5', '6', '1', '1', '117704965', '5', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Voreri', '7', '8', '1', '1', '262912', '5', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Brarebaen', '6', '7', '0', '1', '17563912', '3', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ulannor', '10', '5', '0', '1', '100794889', '0', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adwican', '6', '7', '0', '1', '459789', '6', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Crerald', '6', '7', '0', '1', '262672', '4', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cei', '11', '1', '0', '1', '17106944', '6', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ceriwen', '2', '1', '1', '1', '67333', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Umirebeth', '3', '1', '1', '1', '117506310', '9', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Alerawen', '4', '4', '1', '1', '329216', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Alaleven', '2', '4', '1', '1', '33752579', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Belawin', '4', '5', '0', '1', '17039360', '5', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Fadia', '8', '1', '1', '1', '16909060', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adwigo', '6', '3', '0', '1', '33882625', '6', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aroreri', '5', '5', '1', '1', '100927489', '7', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dryand', '3', '6', '0', '1', '117507076', '0', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Hyllan', '4', '11', '1', '1', '65794', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Yeralla', '8', '8', '1', '1', '84083714', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Trald', '2', '4', '0', '1', '100991744', '7', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Agrigomma', '8', '7', '1', '1', '100795138', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Mendaf', '6', '7', '0', '1', '17172241', '4', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Feiniel', '8', '1', '1', '1', '100860674', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Unedda', '10', '3', '1', '1', '151715849', '9', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Seveng', '4', '11', '1', '1', '84018184', '2', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Larori', '6', '7', '1', '1', '34014477', '1', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Griavudd', '3', '5', '0', '1', '117769987', '5', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Wicaliwin', '2', '1', '0', '1', '50528769', '6', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Preikon', '6', '1', '0', '1', '33818370', '4', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Astayr', '4', '6', '0', '1', '101057536', '2', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Trilan', '5', '5', '1', '1', '117964802', '2', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gies', '2', '1', '0', '1', '67371269', '6', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Trikith', '5', '9', '0', '1', '118295557', '3', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ethian', '3', '1', '1', '1', '84149512', '1', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Breliseth', '3', '3', '0', '1', '151584770', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Alemos', '3', '1', '0', '1', '394502', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Waniel', '11', '6', '1', '1', '69130', '7', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Thiralath', '1', '8', '0', '1', '100860674', '3', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aferihar', '10', '3', '0', '1', '100862977', '8', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Rhiradia', '8', '1', '1', '1', '84017923', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Qeriseth', '3', '5', '0', '1', '117836032', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Uleawen', '4', '3', '1', '1', '16909575', '5', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eowirien', '4', '4', '1', '1', '198661', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Foma', '8', '4', '0', '1', '16843009', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Jeriredon', '3', '2', '0', '1', '134808068', '5', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dreri', '6', '7', '0', '1', '131849', '3', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Elymos', '3', '1', '0', '1', '134676484', '9', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adoref', '11', '1', '0', '1', '17106442', '4', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zarella', '4', '11', '1', '1', '17172483', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Legewen', '8', '6', '1', '1', '100794627', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kaoaviel', '7', '1', '1', '1', '134479872', '3', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Onuwiel', '1', '6', '1', '1', '17436423', '0', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gwalia', '6', '1', '1', '1', '16975119', '1', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Froejar', '8', '8', '0', '1', '100860931', '2', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Sevardonnon', '11', '5', '0', '1', '17041412', '1', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Onirahar', '4', '11', '0', '1', '117768963', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aceliclya', '10', '9', '1', '1', '131843', '0', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Frarelin', '7', '8', '0', '1', '134284545', '3', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Legilang', '2', '1', '1', '1', '33686277', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lothilag', '4', '4', '0', '1', '50597120', '1', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Olaedien', '11', '2', '1', '1', '50791947', '4', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Edilari', '6', '1', '1', '1', '17565707', '2', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Konnon', '2', '9', '0', '1', '17105155', '2', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lelakin', '3', '5', '0', '1', '151456007', '5', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aceali', '8', '7', '0', '1', '83951875', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Wedrid', '1', '1', '0', '1', '50661380', '4', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Diremwen', '3', '2', '1', '1', '117637127', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Agreiweth', '7', '8', '1', '1', '84018179', '2', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Jeralelind', '6', '1', '1', '1', '17170705', '2', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Oloawiel', '4', '3', '1', '1', '117572357', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Rei', '10', '9', '0', '1', '34211331', '8', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dwirenia', '2', '4', '1', '1', '16843521', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Larerraldan', '11', '6', '0', '1', '84082955', '0', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Nydoewen', '4', '4', '1', '1', '17039624', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cadaydus', '1', '1', '0', '1', '134550276', '0', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Faeron', '3', '5', '0', '1', '34014983', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cilimas', '11', '3', '0', '1', '100861451', '4', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kiraria', '5', '5', '1', '1', '50595072', '8', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Rhalitrem', '6', '1', '0', '1', '33947659', '2', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kaauthiel', '4', '4', '1', '1', '117705992', '1', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adalecia', '3', '4', '1', '1', '50921480', '1', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eraot', '1', '1', '0', '1', '67308295', '1', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Larigowen', '8', '7', '1', '1', '67305987', '5', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Chemwen', '6', '11', '1', '1', '17041163', '6', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Calelith', '1', '1', '1', '1', '84870146', '4', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Haeaa', '8', '1', '0', '1', '16843524', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Haeriwen', '10', '8', '1', '1', '17435655', '4', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Seveang', '5', '6', '1', '1', '17236996', '2', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Briladith', '8', '7', '1', '1', '67174658', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Loamar', '5', '6', '0', '1', '67241216', '3', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gli', '7', '1', '0', '1', '65796', '1', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lethien', '7', '6', '1', '1', '84148739', '4', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ziracan', '4', '11', '0', '1', '16974081', '4', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kedoeni', '10', '5', '1', '1', '151325704', '6', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Etherac', '5', '1', '0', '1', '67700484', '12', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cria', '8', '5', '0', '1', '151126785', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Chilal', '10', '4', '0', '1', '66823', '1', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Froeli', '3', '5', '0', '1', '118030854', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Craen', '7', '4', '1', '1', '131585', '6', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Elardonna', '6', '6', '1', '1', '17039883', '4', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Duvudd', '4', '4', '0', '1', '100993026', '4', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eowaetram', '2', '1', '0', '1', '16909573', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Thilini', '7', '4', '1', '1', '262658', '5', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ethoitram', '5', '5', '0', '1', '17107971', '3', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gweriwen', '5', '8', '1', '1', '67175427', '1', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eroasien', '2', '1', '1', '1', '117704709', '3', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Qerithien', '2', '7', '1', '1', '117637891', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Galireniel', '8', '6', '1', '1', '67175426', '10', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Umiev', '5', '9', '0', '1', '84478209', '5', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Saedda', '3', '5', '1', '1', '84411395', '1', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Onerathiel', '11', '7', '1', '1', '200204', '4', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aleravia', '3', '6', '1', '1', '118032132', '1', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zis', '4', '5', '0', '1', '100993025', '1', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Nydelash', '6', '1', '0', '1', '460032', '4', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Yoawen', '7', '1', '1', '1', '16909571', '3', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zelameth', '10', '6', '1', '1', '134940166', '9', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Theiwen', '11', '6', '1', '1', '84413959', '0', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Vodus', '7', '8', '0', '1', '33685508', '0', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lothani', '10', '3', '1', '1', '16910337', '2', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Nyderakith', '6', '11', '0', '1', '788239', '0', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Criewien', '5', '4', '1', '1', '34080515', '12', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Praema', '3', '6', '1', '1', '83953410', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Umeasa', '10', '9', '1', '1', '117506564', '0', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ocieand', '4', '11', '0', '1', '50529538', '4', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Vaynia', '5', '8', '1', '1', '100991488', '4', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gwaewan', '10', '4', '0', '1', '151520007', '4', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Afoikor', '2', '6', '0', '1', '50595585', '7', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Sevela', '10', '6', '0', '1', '135006466', '8', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Astuwin', '1', '1', '0', '1', '184616198', '3', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Grialla', '1', '2', '1', '1', '118031366', '7', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gralesa', '5', '4', '1', '1', '67437570', '4', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Glawia', '11', '5', '1', '1', '67503878', '7', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ibesh', '1', '9', '0', '1', '33687552', '8', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Legella', '4', '11', '1', '1', '50466054', '1', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Traukoth', '6', '7', '0', '1', '263435', '2', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Edymma', '6', '6', '1', '1', '33947657', '5', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Veilith', '1', '1', '0', '1', '84806152', '1', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cerracia', '2', '1', '1', '1', '84082695', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Mirerrawen', '7', '4', '1', '1', '66561', '4', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Legao', '6', '7', '0', '1', '16908808', '1', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Haaellan', '4', '6', '1', '1', '117835266', '1', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Traossa', '10', '9', '1', '1', '101450243', '0', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cadedith', '10', '4', '1', '1', '33883655', '0', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Glaogan', '8', '4', '0', '1', '33685508', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Nydeacia', '2', '9', '1', '1', '67241477', '8', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Rau', '11', '6', '0', '1', '84346887', '2', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Jirawin', '10', '6', '0', '1', '16974593', '6', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Nydaybard', '10', '8', '0', '1', '100861959', '8', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Boma', '4', '6', '0', '1', '265475', '2', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ethoennor', '11', '7', '0', '1', '100926473', '0', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Peliwen', '1', '5', '1', '1', '151979523', '5', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Glywyn', '11', '5', '0', '1', '67633921', '0', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Therrath', '1', '2', '0', '1', '50860801', '8', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Severirien', '4', '6', '1', '1', '131842', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ibaonyth', '7', '9', '0', '1', '134348803', '1', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Celi', '3', '1', '0', '1', '84150279', '6', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Glera', '5', '6', '0', '1', '525058', '4', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eralisean', '1', '9', '0', '1', '198408', '7', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adwerrarith', '4', '11', '1', '1', '117640450', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Olerrallan', '1', '4', '1', '1', '151060487', '2', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lotheriviel', '2', '9', '1', '1', '17041160', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cadessi', '11', '8', '1', '1', '33882890', '2', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Elaliwiel', '1', '8', '1', '1', '184748548', '2', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adwoadia', '7', '4', '1', '1', '16844288', '3', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dalitram', '7', '4', '0', '1', '84149762', '2', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Vendalla', '7', '1', '1', '1', '262913', '2', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Umeas', '4', '6', '0', '1', '67502080', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Thelawen', '4', '4', '1', '1', '67371781', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Arelili', '6', '11', '0', '1', '329480', '6', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Niran', '8', '8', '0', '1', '16843267', '3', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adrelam', '7', '8', '0', '1', '131843', '0', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gwirep', '11', '5', '0', '1', '84345610', '3', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gwalikor', '5', '5', '0', '1', '100991749', '7', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Legirap', '4', '6', '0', '1', '84020481', '5', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gilagan', '1', '9', '0', '1', '84805379', '4', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kedaeweth', '2', '6', '1', '1', '84083974', '6', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gwarith', '8', '6', '1', '1', '67240962', '5', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kairadda', '10', '8', '1', '1', '151128067', '4', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aselan', '2', '4', '1', '1', '50594311', '4', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adwilia', '10', '9', '1', '1', '84215041', '2', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Arirelian', '1', '1', '1', '1', '101058821', '2', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lotheawia', '4', '5', '1', '1', '50531586', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Grilirin', '8', '1', '0', '1', '100860930', '10', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Chao', '5', '5', '0', '1', '134744324', '14', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ibo', '3', '5', '0', '1', '84345092', '0', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Alilidien', '7', '8', '1', '1', '16908802', '3', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Grelin', '1', '4', '1', '1', '117508869', '0', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cheawia', '7', '1', '1', '1', '17039875', '5', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Brosh', '2', '6', '0', '1', '100730373', '4', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ibalessi', '7', '8', '1', '1', '33620736', '3', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ocilini', '1', '1', '1', '1', '118096384', '8', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Galat', '6', '1', '0', '1', '17105673', '5', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eligobeth', '3', '3', '1', '1', '118032131', '3', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Afaewyn', '6', '7', '0', '1', '132875', '1', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Caduclya', '11', '3', '1', '1', '196865', '7', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zeiwyr', '4', '6', '0', '1', '17105671', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Hairasien', '1', '1', '1', '1', '33882889', '8', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Nydethiel', '7', '4', '1', '1', '83953155', '3', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kaoewan', '4', '11', '0', '1', '50466049', '4', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Galaeven', '10', '5', '1', '1', '101256196', '4', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Thirared', '1', '8', '0', '1', '16975623', '4', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Mireaven', '8', '3', '1', '1', '33883139', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zilillan', '2', '4', '1', '1', '16844545', '4', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eteawen', '1', '2', '1', '1', '51253000', '7', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Hayg', '11', '7', '0', '1', '84348424', '4', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Bae', '2', '9', '0', '1', '83952386', '7', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Pardossi', '8', '5', '1', '1', '117637121', '5', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Thelini', '4', '4', '1', '1', '50463751', '1', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Iberiwen', '10', '9', '1', '1', '84413444', '8', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Fia', '8', '7', '0', '1', '67436545', '10', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Etaymar', '7', '6', '0', '1', '134480387', '0', '-6240', '331', '383', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Cawen', '8', '6', '1', '1', '33751044', '2', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Abeven', '4', '3', '1', '1', '67243267', '0', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Nydendathien', '11', '5', '1', '1', '197377', '0', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Afurid', '8', '1', '0', '1', '67174659', '0', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kaeride', '4', '6', '1', '1', '65541', '2', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Therakor', '6', '1', '0', '1', '262667', '1', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ybesa', '1', '6', '1', '1', '84607753', '5', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Chiradda', '1', '9', '1', '1', '17432581', '3', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Arerilind', '3', '3', '1', '1', '151584768', '6', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Unomam', '3', '1', '0', '1', '526342', '6', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Taymos', '6', '1', '0', '1', '17172227', '1', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Celiwen', '5', '5', '1', '1', '84346372', '2', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ybaewyn', '3', '5', '0', '1', '67848', '6', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Elalimwen', '4', '6', '1', '1', '16843525', '2', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Brumos', '5', '6', '0', '1', '117703426', '0', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Yiethien', '4', '3', '1', '1', '67306242', '0', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aliremwen', '11', '3', '1', '1', '34015232', '6', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Trohawin', '10', '6', '0', '1', '134874375', '6', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aloreldan', '8', '7', '0', '1', '100794369', '2', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Riralind', '5', '5', '1', '1', '50661122', '7', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zohabwyn', '3', '5', '0', '1', '459776', '0', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aigodia', '11', '7', '1', '1', '50859520', '3', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kaukath', '5', '5', '0', '1', '50857989', '3', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Wader', '8', '6', '0', '1', '83951619', '5', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Aeahar', '4', '1', '0', '1', '17041416', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Proenn', '1', '2', '0', '1', '84806401', '6', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adraem', '8', '3', '0', '1', '16974338', '1', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Teadith', '2', '7', '1', '1', '50529798', '6', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Areraria', '5', '4', '1', '1', '33950722', '2', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Agrelalin', '3', '2', '0', '1', '134676736', '0', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Telaweth', '4', '5', '1', '1', '100925448', '5', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Proreg', '1', '4', '0', '1', '84609545', '8', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dwigokath', '11', '5', '0', '1', '84412427', '1', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Hendameth', '5', '8', '1', '1', '855045', '4', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dithiel', '6', '6', '1', '1', '263185', '3', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Trigossi', '4', '6', '1', '1', '101057798', '4', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adardoswen', '1', '2', '1', '1', '67699719', '4', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Dreni', '3', '1', '1', '1', '117901576', '5', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Yendatha', '1', '5', '0', '1', '17499137', '3', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Firaand', '11', '1', '0', '1', '101253122', '0', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Trerilia', '11', '1', '1', '1', '17170953', '2', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Zayswen', '5', '4', '1', '1', '118294785', '1', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Rirader', '6', '3', '0', '1', '459784', '2', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adauwiel', '4', '3', '1', '1', '50397185', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Kedigodric', '1', '6', '0', '1', '34472710', '6', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ociabard', '11', '6', '0', '1', '67567627', '1', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ybaot', '4', '11', '0', '1', '67503112', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Grauven', '3', '6', '1', '1', '50791687', '0', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Thaywan', '11', '6', '0', '1', '33816844', '3', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Prielia', '2', '4', '1', '1', '33818112', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gaejan', '4', '1', '0', '1', '117768448', '2', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Migog', '10', '6', '0', '1', '151520519', '4', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Onearith', '3', '6', '1', '1', '263944', '1', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Asterild', '1', '9', '0', '1', '722433', '4', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Sevaukith', '4', '6', '0', '1', '17105925', '2', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Jealin', '3', '4', '0', '1', '33753092', '10', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Gidia', '6', '1', '1', '1', '17039364', '3', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Grean', '10', '9', '1', '1', '459778', '3', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Eowerram', '4', '5', '0', '1', '16975879', '6', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adoeric', '5', '9', '0', '1', '151454721', '5', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Doa', '8', '4', '0', '1', '67306243', '9', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Sevoach', '6', '3', '0', '1', '460813', '6', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Haakoth', '1', '8', '0', '1', '17695496', '1', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Jire', '5', '9', '0', '1', '34080514', '5', '1676.71', '1678.31', '121.67', '0', '0', '11', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Praenn', '6', '3', '0', '1', '17040388', '3', '-2917.58', '-257.98', '52.9968', '1', '0', '22', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Raliwiel', '10', '4', '1', '1', '118162947', '3', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Agraviel', '1', '1', '1', '1', '117772035', '6', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Thaung', '2', '3', '1', '1', '84084486', '3', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Etelawyn', '2', '9', '0', '1', '17040641', '7', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Lirabwyn', '3', '3', '0', '1', '151062279', '6', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Praliwyn', '10', '9', '0', '1', '117639431', '1', '10349.6', '-6357.29', '33.4026', '530', '0', '82', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ybeadia', '3', '3', '1', '1', '151521027', '0', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Groiswen', '1', '9', '1', '1', '529155', '1', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Acewen', '1', '5', '1', '1', '84872969', '0', '-8949.95', '-132.493', '83.5312', '0', '0', '2', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Ocalen', '11', '5', '0', '1', '50922244', '1', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Praand', '2', '1', '0', '1', '50529541', '3', '-618.518', '-4251.67', '38.718', '1', '0', '23', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adweatrem', '4', '3', '0', '1', '17170693', '3', '10311.3', '832.463', '1326.41', '1', '0', '27', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Adrardossa', '11', '7', '1', '1', '16974598', '4', '-3961.64', '-13931.2', '100.615', '530', '0', '94', '1');

INSERT INTO `characters` (`account`, `name`, `race`, `class`, `gender`, `level`, `playerBytes`, `playerBytes2`, `position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `health`)
VALUES ('1', 'Sevelich', '3', '2', '0', '1', '34080003', '1', '-6240.32', '331.033', '382.758', '0', '0', '6', '1');

