DELETE FROM achievement_criteria_requirement WHERE criteria_id IN (3685,3879,3880,7020,7021);
INSERT INTO achievement_criteria_requirement (criteria_id,TYPE,value1) VALUES
-- Stormtrooper
(3685,6,3820),
(3685,7,34976),
-- Take a Chill Pill
(3879,6,3820),
(3879,7,23505),
-- Not So Fast
(3880,6,3277),
(3880,7,23451),
-- Not In My House
(7020,6,3321),
(7020,7,23335),
(7021,6,3320),
(7021,7,23333);

-- We Had It All Along *cough*
UPDATE achievement_criteria_requirement SET value1 = 1590, value2 = 1590 WHERE criteria_id = 1235;

-- Children' Week achievements
DELETE FROM achievement_criteria_requirement WHERE criteria_id IN (6651,6652,6653,6654,6655,6656,6657,12398,6659,10391,6641,6642,6643,6644);
INSERT INTO achievement_criteria_requirement (criteria_id,TYPE,value1) VALUES
(6651,16,201),
(6652,16,201),
(6653,16,201),
(6654,16,201),
(6655,16,201),
(6656,16,201),
(6657,16,201),
(12398,16,201),
(6659,16,201),
(10391,16,201),
(6641,16,201),
(6642,16,201),
(6643,16,201),
(6644,16,201);