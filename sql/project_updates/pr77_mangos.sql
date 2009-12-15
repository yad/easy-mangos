delete FROM spell_proc_event WHERE entry IN (21084, 31801);
insert into spell_proc_event (entry,SchoolMask) values (21084,1);
insert into spell_proc_event (entry,SchoolMask) values (31801,1);
update spell_proc_event set ppmRate=0,SchoolMask=1 where entry=20375;
delete from spell_bonus_data where entry in (20167,20187,31803,31893,31898,32220,32221,53718,53719,53725,53726,53742);