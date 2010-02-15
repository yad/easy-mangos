DROP TABLE IF EXISTS `jail_conf`;
SET FOREIGN_KEY_CHECKS=0;

CREATE TABLE `jail_conf` (
  `id` int(11) NOT NULL auto_increment,
  `obt` varchar(50) default NULL,
  `jail_conf` int(11) default NULL,
  `jail_tele` float default NULL,
  `help_enq` varchar(255) default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=17 DEFAULT CHARSET=utf8;

INSERT INTO `jail_conf` VALUES ('1', 'm_jailconf_max_jails', '3', null, '');
INSERT INTO `jail_conf` VALUES ('2', 'm_jailconf_max_duration', '672', null, '');
INSERT INTO `jail_conf` VALUES ('3', 'm_jailconf_min_reason', '25', null, '');
INSERT INTO `jail_conf` VALUES ('4', 'm_jailconf_warn_player', '1', null, '');
INSERT INTO `jail_conf` VALUES ('5', 'm_jailconf_amnestie', '180', null, '');
INSERT INTO `jail_conf` VALUES ('6', 'm_jailconf_ally_x', null, '31.7282', '');
INSERT INTO `jail_conf` VALUES ('7', 'm_jailconf_ally_y', null, '135.794', '');
INSERT INTO `jail_conf` VALUES ('8', 'm_jailconf_ally_z', null, '-40.0508', '');
INSERT INTO `jail_conf` VALUES ('9', 'm_jailconf_ally_o', null, '4.73516', '');
INSERT INTO `jail_conf` VALUES ('10', 'm_jailconf_ally_m', '35', null, '');
INSERT INTO `jail_conf` VALUES ('11', 'm_jailconf_horde_x', null, '2179.85', '');
INSERT INTO `jail_conf` VALUES ('12', 'm_jailconf_horde_y', null, '-4763.96', '');
INSERT INTO `jail_conf` VALUES ('13', 'm_jailconf_horde_z', null, '54.911', '');
INSERT INTO `jail_conf` VALUES ('14', 'm_jailconf_horde_o', null, '4.44216', '');
INSERT INTO `jail_conf` VALUES ('15', 'm_jailconf_horde_m', '1', null, '');
INSERT INTO `jail_conf` VALUES ('16', 'm_jailconf_ban', '0', null, '');
INSERT INTO `jail_conf` VALUES ('17', 'm_jailconf_radius', '10', null, '');

DROP TABLE IF EXISTS `jail`;
SET FOREIGN_KEY_CHECKS=0;

CREATE TABLE `jail` (
  `guid` int(11) unsigned NOT NULL COMMENT 'GUID of the jail brother',
  `char` varchar(13) NOT NULL COMMENT 'Jailed charname',
  `release` int(11) unsigned NOT NULL COMMENT 'Release time for the char',
  `amnestietime` int(11) NOT NULL,
  `reason` varchar(255) NOT NULL COMMENT 'Reason for the jail',
  `times` int(11) unsigned NOT NULL COMMENT 'How many times this char already was jailed',
  `gmacc` int(11) unsigned NOT NULL COMMENT 'Used GM acc to jail this char',
  `gmchar` varchar(13) NOT NULL COMMENT 'Used GM char to jail this char',
  `lasttime` timestamp NOT NULL default '0000-00-00 00:00:00' on update CURRENT_TIMESTAMP COMMENT 'Last time jailed',
  `duration` int(11) unsigned NOT NULL default '0' COMMENT 'Duration of the jail',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Jail table for MaNGOS by WarHead';