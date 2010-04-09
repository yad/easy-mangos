DELETE FROM spell_chain WHERE spell_id IN (49018, 49529, 49530);
INSERT INTO spell_chain VALUES
(49018, 0, 49018, 1, 0),
(49529, 49018, 49018, 2, 0),
(49530, 49529, 49018, 3, 0);
DELETE FROM spell_proc_event WHERE entry = 49018;
INSERT INTO spell_proc_event VALUES
(49018, 0, 15, 20971520, 0, 0, 0, 0, 0, 0, 0);