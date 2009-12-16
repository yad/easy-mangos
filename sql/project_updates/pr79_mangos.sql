-- Vampiric Touch Dispell - 40% SP coefficient
DELETE FROM spell_bonus_data WHERE entry IN (64085);
INSERT INTO spell_bonus_data VALUES
(64085 , 0.4, 0, 0, "Priest - Vampiric Touch Dispell");

-- Glyph of Dispel Magic
DELETE FROM spell_bonus_data WHERE entry IN (56131);
INSERT INTO spell_bonus_data VALUES
(56131 , 0, 0, 0, "Priest - Glyph of Dispel Magic");