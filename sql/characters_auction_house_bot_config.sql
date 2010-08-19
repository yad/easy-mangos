/*
Navicat MySQL Data Transfer

Source Server         : Serveur non virtualiusé
Source Server Version : 50045
Source Host           : 192.168.1.212:3306
Source Database       : characters

Target Server Type    : MYSQL
Target Server Version : 50045
File Encoding         : 65001

Date: 2010-08-19 02:16:24
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `auction_house_bot_config`
-- ----------------------------
DROP TABLE IF EXISTS `auction_house_bot_config`;
CREATE TABLE `auction_house_bot_config` (
  `auctionhouse` int(11) NOT NULL default '0' COMMENT 'mapID of the auctionhouse.',
  `name` char(25) default NULL COMMENT 'Text name of the auctionhouse.',
  `mintime` int(11) default '8' COMMENT 'Sets the minimum number of hours for an auction.',
  `maxtime` int(11) default '24' COMMENT 'Sets the maximum number of hours for an auction.',
  `greyitems` int(11) default '0' COMMENT 'Sets the amount of the Grey auction items',
  `whiteitems` int(11) default '2500' COMMENT 'Sets the amount of the White auction items',
  `greenitems` int(11) default '3500' COMMENT 'Sets the amount of the Green auction items',
  `blueitems` int(11) default '1500' COMMENT 'Sets the amount of Blue auction items',
  `purpleitems` int(11) default '800' COMMENT 'Sets the amount of the Purple auction items',
  `orangeitems` int(11) default '1' COMMENT 'Sets the amount of the Orange auction items',
  `yellowitems` int(11) default '0' COMMENT 'Sets the amount of the Yellow auction items',
  `minpricegrey` int(11) default '100' COMMENT 'Minimum price of Grey items (percentage).',
  `maxpricegrey` int(11) default '150' COMMENT 'Maximum price of Grey items (percentage).',
  `minpricewhite` int(11) default '150' COMMENT 'Minimum price of White items (percentage).',
  `maxpricewhite` int(11) default '250' COMMENT 'Maximum price of White items (percentage).',
  `minpricegreen` int(11) default '800' COMMENT 'Minimum price of Green items (percentage).',
  `maxpricegreen` int(11) default '1400' COMMENT 'Maximum price of Green items (percentage).',
  `minpriceblue` int(11) default '1250' COMMENT 'Minimum price of Blue items (percentage).',
  `maxpriceblue` int(11) default '1750' COMMENT 'Maximum price of Blue items (percentage).',
  `minpricepurple` int(11) default '2250' COMMENT 'Minimum price of Purple items (percentage).',
  `maxpricepurple` int(11) default '4550' COMMENT 'Maximum price of Purple items (percentage).',
  `minpriceorange` int(11) default '3250' COMMENT 'Minimum price of Orange items (percentage).',
  `maxpriceorange` int(11) default '5550' COMMENT 'Maximum price of Orange items (percentage).',
  `minpriceyellow` int(11) default '5250' COMMENT 'Minimum price of Yellow items (percentage).',
  `maxpriceyellow` int(11) default '6550' COMMENT 'Maximum price of Yellow items (percentage).',
  `minbidpricegrey` int(11) default '70' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 70',
  `maxbidpricegrey` int(11) default '100' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpricewhite` int(11) default '70' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 70',
  `maxbidpricewhite` int(11) default '100' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpricegreen` int(11) default '80' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpricegreen` int(11) default '100' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpriceblue` int(11) default '75' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 75',
  `maxbidpriceblue` int(11) default '100' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpricepurple` int(11) default '80' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpricepurple` int(11) default '100' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpriceorange` int(11) default '80' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpriceorange` int(11) default '100' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpriceyellow` int(11) default '80' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpriceyellow` int(11) default '100' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 100',
  `maxstackgrey` int(11) default '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackwhite` int(11) default '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackgreen` int(11) default '3' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackblue` int(11) default '2' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackpurple` int(11) default '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackorange` int(11) default '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackyellow` int(11) default '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `buyerpricegrey` int(11) default '1' COMMENT 'Multiplier to vendorprice when buying grey items from auctionhouse',
  `buyerpricewhite` int(11) default '1' COMMENT 'Multiplier to vendorprice when buying white items from auctionhouse',
  `buyerpricegreen` int(11) default '5' COMMENT 'Multiplier to vendorprice when buying green items from auctionhouse',
  `buyerpriceblue` int(11) default '12' COMMENT 'Multiplier to vendorprice when buying blue items from auctionhouse',
  `buyerpricepurple` int(11) default '15' COMMENT 'Multiplier to vendorprice when buying purple items from auctionhouse',
  `buyerpriceorange` int(11) default '20' COMMENT 'Multiplier to vendorprice when buying orange items from auctionhouse',
  `buyerpriceyellow` int(11) default '22' COMMENT 'Multiplier to vendorprice when buying yellow items from auctionhouse',
  `buyerbiddinginterval` int(11) default '1' COMMENT 'Interval how frequently AHB bids on each AH. Time in minutes',
  `buyerbidsperinterval` int(11) default '1' COMMENT 'number of bids to put in per bidding interval',
  PRIMARY KEY  (`auctionhouse`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of auction_house_bot_config
-- ----------------------------
INSERT INTO `auction_house_bot_config` VALUES ('2', 'Alliance', '8', '24', '0', '2500', '3500', '1500', '800', '1', '0', '100', '150', '150', '250', '800', '1400', '1250', '1750', '2250', '4550', '3250', '5550', '5250', '6550', '70', '100', '70', '100', '80', '100', '75', '100', '80', '100', '80', '100', '80', '100', '0', '0', '3', '2', '1', '1', '1', '1', '1', '5', '12', '15', '20', '22', '1', '1');
INSERT INTO `auction_house_bot_config` VALUES ('6', 'Horde', '8', '24', '0', '2500', '3500', '1500', '800', '1', '0', '100', '150', '150', '250', '800', '1400', '1250', '1750', '2250', '4550', '3250', '5550', '5250', '6550', '70', '100', '70', '100', '80', '100', '75', '100', '80', '100', '80', '100', '80', '100', '0', '0', '3', '2', '1', '1', '1', '1', '1', '5', '12', '15', '20', '22', '1', '1');
INSERT INTO `auction_house_bot_config` VALUES ('7', 'Neutral', '8', '24', '0', '2500', '3500', '1500', '800', '1', '0', '100', '150', '150', '250', '800', '1400', '1250', '1750', '2250', '4550', '3250', '5550', '5250', '6550', '70', '100', '70', '100', '80', '100', '75', '100', '80', '100', '80', '100', '80', '100', '0', '0', '3', '2', '1', '1', '1', '1', '1', '5', '12', '15', '20', '22', '1', '1');
