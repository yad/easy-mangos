UPDATE quest_template SET ReqSpellCast1 = 0, ReqSpellCast2 = 0, ReqSpellCast3 = 0, ReqSpellCast4 = 0 WHERE entry = 12641;
UPDATE creature_template SET modelid_1 = 16925, modelid_3 = 16925, scale = 2, flags_extra = 0 WHERE entry IN (28525,28542,28543,28544);
UPDATE creature_template SET ScriptName = 'npc_eye_of_acherus', modelid_3 = 26320 WHERE entry = 28511;
UPDATE creature_model_info SET modelid_other_gender = 0 WHERE modelid = 26320;
REPLACE INTO creature_template_addon (`entry`,`moveflags`,`auras`) VALUES (28511,33562624,''),(28525,0,'64328 0'),(28542,0,'64328 0'),(28543,0,'64328 0'),(28544,0,'64328 0');
REPLACE INTO spell_script_target (`entry`,`type`,`targetEntry`) VALUES (51859,1,28525),(51859,1,28542),(51859,1,28543),(51859,1,28544);
DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id IN (28511,28525,28542,28543,28544));
UPDATE creature_template SET InhabitType = 3 WHERE entry = 28511;
