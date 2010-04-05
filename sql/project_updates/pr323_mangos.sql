DROP TABLE IF EXISTS `spell_stack_class_data`; 
CREATE TABLE `spell_stack_class_data` ( 
  `entry` mediumint(8) unsigned NOT NULL, 
  `stack_conditions` mediumint(9) default '0', 
  `value` mediumint(9) default '0', 
  `comments` varchar(255) default NULL, 
   PRIMARY KEY  (`entry`) 
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Spell stacking behavior'; 
 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('0','0','0','Default class - it does nothing'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('1','4','0','+X attack power buffs (Blessing of Might...)'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('2','4','0','+X% all stats (Blessing of Kings...)'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('3','4','0','+X% MP5 (Blessing of Wisdom...)'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('4','4','0','+X stamina (Power Word: Fortitude...)'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('5','4','0','+X intellect (Arcane Intellect...)'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('6','4','0','Judgement of Wisdom debuff '); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('7','4','0','Judgement of Light debuff'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('8','4','0','Judgement of Justice debuff'); 
insert into `spell_stack_class_data` (`entry`, `stack_conditions`, `value`, `comments`) values('9','2','0','Judgement debuffs (not needed - example)');

DROP TABLE IF EXISTS `spell_stack_data`; 
CREATE TABLE `spell_stack_data` ( 
  `entry` mediumint(8) unsigned NOT NULL, 
  `stack_class1` mediumint(9) default '0', 
  `stack_class2` mediumint(9) default '0', 
  `stack_class3` mediumint(9) default '0', 
   PRIMARY KEY  (`entry`) 
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Spell stacking conditions'; 
 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('19740','1','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('6673','1','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('25782','1','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('25898','2','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('20217','2','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('72586','2','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('25894','3','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('1459','5','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('61024','5','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('61316','5','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('23028','5','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('1243','4','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('21562','4','0','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('20185','7','9','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('20184','8','9','0'); 
insert into `spell_stack_data` (`entry`, `stack_class1`, `stack_class2`, `stack_class3`) values('20186','6','9','0');

