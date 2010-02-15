DELETE FROM spell_bonus_data WHERE entry IN (1978, 3044, 42243, 53352, 56641);
INSERT INTO spell_bonus_data VALUES
(1978, 0, 1, 0.04, "Hunter - Serpent Sting"),
(3044, 1, 0, 0.15, "Hunter - Arcane Shot"),
(42243, 1, 0, 0.01395, "Hunter - Volley" ),
(53352, 1, 0, 0.14, "Hunter - Explosive Shot"),
(56641, 1, 0, 0.1, "Hunter - Steady Shot");

DELETE FROM spell_chain WHERE spell_id IN (42243, 42244, 42245, 42234, 58432, 58433);
INSERT INTO spell_chain VALUES
(42243, 0, 42243, 1, 0),
(42244, 42243, 42243, 2, 0),
(42245, 42244, 42243, 3, 0),
(42234, 42245, 42243, 4, 0),
(58432, 42234, 42243, 5, 0),
(58433, 58432, 42243, 6, 0);