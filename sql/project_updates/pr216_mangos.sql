DELETE FROM spell_bonus_data WHERE entry IN (SELECT spell_id FROM spell_chain WHERE first_spell IN ( 467, 7294));
INSERT INTO spell_bonus_data VALUES
(467, 0.033, 0, 0, "Druid -Thorns"),
(7294, 0.033, 0, 0, "Paladin - Retribution Aura");