CREATE TABLE IF NOT EXISTS `character_loginequip` (
  `class` tinyint(3) NOT NULL DEFAULT '0',
  `level` tinyint(3) NOT NULL DEFAULT '1',
  `slot` tinyint(3) NOT NULL DEFAULT '0',
  `entry` int(11) NOT NULL DEFAULT '0',
  `count` tinyint(3) NOT NULL DEFAULT '1',
  KEY (`class`),
  KEY `level` (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `character_learnspells` (
`class` TINYINT( 3 ) NOT NULL DEFAULT '0',
`team` INT( 11 ) NOT NULL DEFAULT '0',
`trainer_entry` INT( 11 ) NOT NULL DEFAULT '0',
`spell` INT( 11 ) NOT NULL DEFAULT '0',
KEY ( `class` ) ,
INDEX ( `team` ) 
) ENGINE = InnoDB;
