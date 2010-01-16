/* Fixes commited by m33ts4k0z */
/* Priests 3.3.0 -- Staves Spell clean-up */
DELETE FROM playercreateinfo_spell WHERE spell=227 AND class=5;

/* Priests Creation - 3.3.0 -- Staves */
INSERT INTO playercreateinfo_spell VALUES (1,5,227,'Staves'),(8,5,227,'Staves'),(10,5,227,'Staves'),(11,5,227,'Staves'),(5,5,227,'Staves'),(4,5,227,'Staves'),(3,5,227,'Staves');

/* Insert new item "Worn Greatsword" for Warrior creation - 3.3.0 */
INSERT INTO item_template VALUES (49778,2,8,-1,'Worn Greatsword',2380,1,0,0,1,45,9,17,-1,-1,2,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,5,0,0,0,0,0,0,0,0,0,0,0,2900,0,0,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,'',0,0,0,0,0,1,1,0,0,0,0,25,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,'',0,0,0,0);


/* Warriors 3.3.0 -- Two-Handed Swords Spell clean-up */
DELETE FROM playercreateinfo_spell WHERE spell=202 AND class=1;


/* Warriors Creation - 3.3.0 -- Two-Handed Swords */
INSERT INTO playercreateinfo_spell VALUES (1,1,202,'Two-Handed Swords'),(2,1,202,'Two-Handed Swords'),(3,1,202,'Two-Handed Swords'),(5,1,202,'Two-Handed Swords'),(11,1,202,'Two-Handed Swords'),(6,1,202,'Two-Handed Swords'),(7,1,202,'Two-Handed Swords'),(8,1,202,'Two-Handed Swords'),(4,1,202,'Two-Handed Swords');

/* Hunter 3.3.0 -- Two-Handed Axes Spell clean-up */
DELETE FROM playercreateinfo_spell WHERE spell=197 AND class=3;

/* Hunters Creation - 3.3.0 -- Two-Handed Axes */
INSERT INTO playercreateinfo_spell VALUES (10,3,197,'Two-Handed Axes'),(2,3,197,'Two-Handed Axes'),(11,3,197,'Two-Handed Axes'),(8,3,197,'Two-Handed Axes'),(4,3,197,'Two-Handed Axes'),(6,3,197,'Two-Handed Axes'),(3,3,197,'Two-Handed Axes');


/* Warlocks 3.3.0 -- Staves Spell clean-up */
DELETE FROM playercreateinfo_spell WHERE spell=227 AND class=9;

/* Warlocks Creation - 3.3.0 -- Staves */
INSERT INTO playercreateinfo_spell VALUES (1,9,227,'Staves'),(5,9,227,'Staves'),(10,9,227,'Staves'),(7,9,227,'Staves'),(2,9,227,'Staves');


/* Insert new item "Worn Dirk" for Rogue creation - 3.3.0 */
INSERT INTO item_template VALUES (50055,2,15,-1,'Worn Dirk',6442,1,0,0,1,36,7,13,-1,-1,2,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,1600,0,0,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,'',0,0,0,0,0,1,3,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,'',0,0,0,0);


/* Rogues 3.3.0 -- Dual Wield Spell clean-up */
DELETE FROM playercreateinfo_spell WHERE spell=674 AND class=4;

/* Rogues Creation - 3.3.0 -- Dual Wield */
INSERT INTO playercreateinfo_spell VALUES (8,4,674,'Dual Wield'),(5,4,674,'Dual Wield'),(10,4,674,'Dual Wield'),(7,4,674,'Dual Wield'),(2,4,674,'Dual Wield'),(1,4,674,'Dual Wield'),(4,4,674,'Dual Wield'),(3,4,674,'Dual Wield');

/* Fixes commited by m33ts4k0z */
/* Several item prototypes startup error fixes -- 3.3.0 */ 

UPDATE item_template SET displayid=63370 WHERE Entry=1604;
UPDATE item_template SET displayid=7414 WHERE Entry=17020;
UPDATE item_template SET displayid=29165 WHERE Entry=17021;
UPDATE item_template SET displayid=7406 WHERE Entry=17026;
UPDATE item_template SET displayid=23717 WHERE Entry=17759;
UPDATE item_template SET displayid=7414 WHERE Entry=20028;
UPDATE item_template SET displayid=45437 WHERE Entry=23822;
UPDATE item_template SET displayid=64903 WHERE Entry=28220;
UPDATE item_template SET displayid=63412 WHERE Entry=30171;
UPDATE item_template SET displayid=63411 WHERE Entry=30190;
UPDATE item_template SET displayid=43501 WHERE Entry=31110;
UPDATE item_template SET displayid=45437 WHERE Entry=33092;
UPDATE item_template SET displayid=64114 WHERE Entry=38409;
UPDATE item_template SET displayid=53251 WHERE Entry=38729;
UPDATE item_template SET displayid=64904 WHERE Entry=41013;
UPDATE item_template SET displayid=64922 WHERE Entry=41014;
UPDATE item_template SET displayid=64904 WHERE Entry=41019;
UPDATE item_template SET displayid=64922 WHERE Entry=41020;
UPDATE item_template SET displayid=62988 WHERE Entry=41133;
UPDATE item_template SET displayid=64904 WHERE Entry=41151;
UPDATE item_template SET displayid=64922 WHERE Entry=41152;
UPDATE item_template SET displayid=64140 WHERE Entry=41166;
UPDATE item_template SET displayid=63408 WHERE Entry=41993;
UPDATE item_template SET displayid=50905 WHERE Entry=43870;
UPDATE item_template SET displayid=65031 WHERE Entry=45470;
UPDATE item_template SET displayid=63410 WHERE Entry=47511;
UPDATE item_template SET displayid=64902 WHERE Entry=49326;
UPDATE item_template SET displayid=64902 WHERE Entry=49327;
UPDATE item_template SET displayid=64902 WHERE Entry=49471;
UPDATE item_template SET displayid=64902 WHERE Entry=49472;
UPDATE item_template SET displayid=64902 WHERE Entry=47511;
UPDATE item_template SET displayid=64902 WHERE Entry=49473;
UPDATE item_template SET displayid=63410 WHERE Entry=47511;
UPDATE item_template SET displayid=64902 WHERE Entry=49328;
UPDATE item_template SET class=15 WHERE Entry IN (3711,20928,20929,20930,20932,20933);
UPDATE item_template SET class=4 WHERE Entry=31080;
UPDATE item_template SET displayid=63410 WHERE Entry=47511;
UPDATE item_template SET displayid=64902 WHERE Entry=49328;
UPDATE item_template SET inventorytype=13 WHERE Entry IN (32912,32915,32917,32918,32919,32920);
UPDATE item_template SET subclass=4 WHERE Entry=31080;
UPDATE item_template SET material=4 WHERE Entry IN(10439,21813,21815,21816,21817,21818,21819,21820,21821,21822,21823,22218,22235,22236,22237,22238,22239,22276,22277,22281,22282,23211,23215,23247,23327,23435,34480,36734,41133,47241);
UPDATE item_template SET material=7 WHERE entry IN(22278,22279,22280);
UPDATE item_template SET inventorytype=22 WHERE entry=50055;