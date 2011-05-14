INSERT IGNORE INTO `gossip_menu_option` VALUES ('10371', '0', '0', 'Purchase a Dual Talent Specialization.', '1', '16', '10373', '0', '50099', '0', '10000000', 'Are you sure you wish to purchase a Dual Talent Specialization?', '15', '40', '1', '0', '0', '0', '0', '0', '0');
INSERT IGNORE INTO `gossip_menu` VALUES ('10371', '14391', '0', '0', '0', '0', '0', '0');
INSERT IGNORE INTO `gossip_menu` VALUES ('10373', '14393', '0', '0', '0', '0', '0', '0');
INSERT IGNORE INTO `locales_gossip_menu_option` VALUES ('10371', '0', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);
UPDATE `locales_gossip_menu_option` SET `option_text_loc2` = 'Acheter une double spécialisation des talents.', `box_text_loc2` = 'Etes-vous $gsûr:sûre; de vouloir acheter votre double spécialisation des talents ?' WHERE `menu_id` = '10371' AND `id` = '0';
UPDATE `locales_gossip_menu_option`, `gossip_menu_option` SET `option_text_loc2` = 'En apprendre plus sur la double spécialisation de talents.' WHERE 
`locales_gossip_menu_option`.`menu_id` = `gossip_menu_option`.`menu_id`
AND
`locales_gossip_menu_option`.`id` = `gossip_menu_option`.`id`
AND
`gossip_menu_option`.`action_menu_id` = '10371';
INSERT IGNORE INTO `gossip_scripts` VALUES ('50099', '0', '15', '63624', '2', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '');