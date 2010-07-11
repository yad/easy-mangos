SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `spell_disabled`
-- ----------------------------
DROP TABLE IF EXISTS `spell_disabled`;
CREATE TABLE `spell_disabled` (
  `entry` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'spell entry',
  `ischeat_spell` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'mark spell as cheat',
  `active` tinyint(3) unsigned NOT NULL DEFAULT '1' COMMENT 'enable check of this spell',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Dissabled Spell System';

-- ----------------------------
-- Records of spell_disabled
-- ----------------------------
INSERT INTO `spell_disabled` VALUES ('5', '0', '1');
INSERT INTO `spell_disabled` VALUES ('47977', '0', '1');
INSERT INTO `spell_disabled` VALUES ('45614', '0', '1');
INSERT INTO `spell_disabled` VALUES ('46432', '0', '1');
INSERT INTO `spell_disabled` VALUES ('605', '0', '1');
INSERT INTO `spell_disabled` VALUES ('60079', '0', '1');
INSERT INTO `spell_disabled` VALUES ('35413', '0', '1');
INSERT INTO `spell_disabled` VALUES ('61254', '0', '1');
INSERT INTO `spell_disabled` VALUES ('34370', '0', '1');
INSERT INTO `spell_disabled` VALUES ('53601', '0', '0');
INSERT INTO `spell_disabled` VALUES ('58597', '0', '0');
INSERT INTO `spell_disabled` VALUES ('65917', '0', '1');
INSERT INTO `spell_disabled` VALUES ('65000', '0', '1');
INSERT INTO `spell_disabled` VALUES ('64999', '0', '1');
INSERT INTO `spell_disabled` VALUES ('50426', '0', '1');
INSERT INTO `spell_disabled` VALUES ('43730', '0', '1');
