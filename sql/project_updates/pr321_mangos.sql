-- This for gossip option for trainers
REPLACE INTO gossip_menu_option (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`, `cond_1`, `cond_1_val_1`, `cond_1_val_2`, `cond_2`, `cond_2_val_1`, `cond_2_val_2`, `cond_3`, `cond_3_val_1`, `cond_3_val_2`) VALUES (0, 16, 0, 'Purchase a Dual Talent Specialization.', 19, 16, 0, 0, 0, 0, 10000000, 'Are you sure you wish to purchase a Dual Talent Specialization?', 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- And this for achievement
REPLACE INTO `achievement_criteria_requirement` (`criteria_id`,`type`,`value1`) VALUES ('9619','0','63624');