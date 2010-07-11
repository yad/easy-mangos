UPDATE `account` SET `gmlevel`= `gmlevel`+1 WHERE `gmlevel` > '0';
UPDATE `command` SET `security`=`security`+1 WHERE `security`>'0';

--revert
UPDATE `account` SET `gmlevel`= `gmlevel`-1 WHERE `gmlevel` > '0';
UPDATE `command` SET `security`=`security`-1 WHERE `security`>'0';
