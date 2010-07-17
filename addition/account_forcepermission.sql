-- ----------------------------
-- Table structure for `account_forcepermission`
-- ----------------------------
DROP TABLE IF EXISTS `account_forcepermission`;
CREATE TABLE `account_forcepermission` (
  `AccountID` int(11) unsigned NOT NULL DEFAULT '0',
  `realmID` int(3) unsigned NOT NULL DEFAULT '0',
  `Security` int(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`AccountID`,`realmID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
