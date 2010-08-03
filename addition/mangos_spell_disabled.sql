DROP TABLE IF EXISTS `spell_disabled`;
CREATE TABLE `spell_disabled` (
  `entry` mediumint(8) unsigned NOT NULL,
  `active` tinyint(3) unsigned NOT NULL default '1',
  `DisabledBy` varchar(255),
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Spell Disabling System';

DELETE FROM `command` WHERE `name` IN ('disable spell', 'enable spell');
INSERT INTO `command` VALUES
('spell disable', 3, 'Syntax: .spell disable #spellid\r\n  Disable spell #spellid.'),
('spell enable',  3, 'Syntax: .spell enable #spellid\r\n  Enable spell #spellid.'),
('spell state',   3, 'Syntax: .spell state #spellid\r\n  Show status for spell with #spellid.');

DELETE FROM `mangos_string` WHERE `entry` IN (11001, 11002, 11003, 11004);
INSERT INTO `mangos_string` VALUES
(11001,'You have disabled spell \'%s\' (id: %u).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,'Вами было заблокировано заклинание \'%s\' (id: %u).'),
(11002,'You have enabled spell \'%s\' (id: %u).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,'Вами было разблокировано заклинание \'%s\' (id: %u).'),
(11003,'Spell \'%s\' (id: %u) disabled by %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,'Заклинание \'%s\' (id: %u) заблокировано %s.'),
(11004,'Spell \'%s\' (id: %u) doesn\'t disabled.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,'Заклинание \'%s\' (id: %u) не заблокировано.');

-- ----------------------------
-- Records of spell_disabled
-- ----------------------------
INSERT INTO `spell_disabled` VALUES ('5',     '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('47977', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('45614', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('46432', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('605',   '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('60079', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('35413', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('61254', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('34370', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('65917', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('65000', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('64999', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('50426', '1', 'ServerAdmin');
INSERT INTO `spell_disabled` VALUES ('43730', '1', 'ServerAdmin');