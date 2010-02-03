DELETE FROM `command` WHERE name IN ('jail','jailinfo','unjail','jailreload','pinfo');
INSERT INTO `command` (name, security, help) VALUES
('jail', 1, 'Syntax: .jail character hours reason\nJailed the \'character\' for \'hours\' with the \'reason\'.'),
('jailinfo', 0, 'Syntax: .jailinfo\nShows your jail status.'),
('unjail', 1, 'Syntax: .unjail character\nRealeases the \'character\' out of the jail.'),
('jailreload', 3, 'Syntax: .jailreload\nLoads the jail config new.'),
('pinfo', 2, 'Syntax: .pinfo [$player_name] [rep] [jail]\nOutput account information for selected player or player find by $player_name.\nIf \'rep\' parameter is provided it shows the reputation information for players.\nIf \'jail\' parameter is provided it shows the jail information for players.');