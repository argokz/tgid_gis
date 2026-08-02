delete from remont2
GO

delete from remont2Deployed
GO

delete from defect where remontID is not null
GO

ALTER TABLE defect add remontNodeID int
GO

ALTER TABLE remont2 add transfer_flag int
GO


insert into remont2 (
stateID, 
data_osmotra, 
data_nachala_plan,
data_okonchaniya_plan,
otchet_po_defektu,
len_tube_plan,
len_izol_plan,
len_channel_plan,
len_tube_cur,
len_izol_cur,
len_channel_cur, 
data_nachala_remonta, 
data_zaversheniya_remonta,
remontTypeID,
remontCatID,
subdivisionID,
responsibleID, 
debug,
transfer_flag
) 
select 
distinct
stateID,
min(data_osmotra) as data_osmotra,
min(data_nachala_remonta) as data_nachala_plan,
max(data_zaversheniya_remonta) as data_okonchaniya_plan,
otchet_po_defektu,
sum(len_tube_cur) as len_tube_plan,
sum(len_izol_cur) as len_izol_plan,
sum(len_channel_cur) as len_channel_plan,
sum(len_tube_cur) as len_tube_cur,
sum(len_izol_cur) as len_izol_cur,
sum(len_channel_cur) as len_channel_cur,
min(data_nachala_remonta) as  data_nachala_remonta, 
max(data_zaversheniya_remonta) as data_zaversheniya_remonta,
max(remontTypeID) as remontTypeID,
max(remontCatID) as remontCatID,
max(subdivisionID)as subdivisionID,
max(responsibleID) as responsibleID,
debug = case
		when t.k > 1 then 2
		when t.k <= 1 then 1
		end,
transfer_flag = 1
from (
select
	r.stateID,
	data_osmotra = case 
		when r.data_osmotra is not null then r.data_osmotra
		when r.data_nachala_remonta is not null then r.data_nachala_remonta
		when r.data_zaversheniya_remonta is not null then r.data_zaversheniya_remonta
	END,
	r.vremya_osmotra,
	r.otchet_po_defektu,
	len_tube_cur = case
		when r.len_tube_cur is not null then r.len_tube_cur
		when r.len_tube_cap is not null then r.len_tube_cap
		when r.len_tube_inv is not null then r.len_tube_inv
	end,
	len_izol_cur = case
		when r.len_izol_cur is not null then r.len_izol_cur
		when r.len_izol_cap is not null then r.len_izol_cap
		when r.len_izol_inv is not null then r.len_izol_inv
	end,
	len_channel_cur = case
		when r.len_channel_cur is not null then r.len_channel_cur
		when r.len_channel_cap is not null then r.len_channel_cap
		when r.len_channel_inv is not null then r.len_channel_inv
	end,
	data_nachala_remonta = case 
		when r.data_nachala_remonta is not null then r.data_nachala_remonta
		when r.data_osmotra is not null then r.data_osmotra
		when r.data_zaversheniya_remonta is not null then r.data_zaversheniya_remonta
	end,
	data_zaversheniya_remonta = case 
		when r.data_zaversheniya_remonta is not null then r.data_zaversheniya_remonta
		when r.data_nachala_remonta is not null then r.data_nachala_remonta
		when r.data_osmotra is not null then r.data_osmotra
	end,
	r.remontTypeID,
	r.remontCatID,
	r.subdivisionID,
	r.responsibleID,
	r.primechanie,
	fr.k
	from remont r
join 
	(select otchet_po_defektu, k  from (
	select r.otchet_po_defektu,count(r.id) as k from remont r
	left join remontTypes rt on rt.id = r.remontTypeID
	group by r.otchet_po_defektu) t
	where t.otchet_po_defektu is not null) fr on fr.otchet_po_defektu = r.otchet_po_defektu
) t
group by 
otchet_po_defektu,
stateID,
t.k
GO

INSERT INTO remont2Deployed (directionID, lineID)
select	
    distinct
	k.directionID,
	k.lineID
	from (
		select
			distinct
				r.id as directionID,
				l.id as lineID,
				l.shape.STDistance(d.shape) as length,
				MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
		from remont d
		JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
		JOIN remont2 r on r.otchet_po_defektu = d.otchet_po_defektu
		LEFT JOIN nodes n1 ON n1.id = l.nodeID1
		LEFT JOIN nodes n2 ON n2.id = l.nodeID2
		left join externalCodes ec1 ON ec1.id = n1.externalCodeID
		left join externalCodes ec2 ON ec2.id = n2.externalCodeID	
		where ( (not ec1.name in ('Ï1','Ï2') or not ec2.name in ('Ï1','Ï2')) or (ec1.name is null AND ec2.name is null) )
	)k
where k.min_len = k.length
order by directionID
GO

insert into defect (lineID, stateID, data_osmotra, vremya_osmotra, 
otchet_po_defektu, len_tube_cur, len_izol_cur, len_channel_cur,
data_nachala_remonta, data_zaversheniya_remonta, remontTypeID, remontCatID, 
subdivisionID, responsibleID, primechanie, shape, transfer_flag, vid_rabotID, remontID, remontNodeID )
select
r.lineID, 
r.stateID,
data_omotra = case 
	when r.data_osmotra is not null then r.data_osmotra
	when r.data_nachala_remonta is not null then r.data_nachala_remonta
	when r.data_zaversheniya_remonta is not null then r.data_zaversheniya_remonta
END,
r.vremya_osmotra,
r.otchet_po_defektu,
len_tube_cur = case
	when r.len_tube_cur is not null then r.len_tube_cur
	when r.len_tube_cap is not null then r.len_tube_cap
	when r.len_tube_inv is not null then r.len_tube_inv
end,
len_izol_cur = case
	when r.len_izol_cur is not null then r.len_izol_cur
	when r.len_izol_cap is not null then r.len_izol_cap
	when r.len_izol_inv is not null then r.len_izol_inv
end,
len_channel_cur = case
	when r.len_channel_cur is not null then r.len_channel_cur
	when r.len_channel_cap is not null then r.len_channel_cap
	when r.len_channel_inv is not null then r.len_channel_inv
end,
data_nachala_remonta = case 
	when r.data_nachala_remonta is not null then r.data_nachala_remonta
	when r.data_osmotra is not null then r.data_osmotra
	when r.data_zaversheniya_remonta is not null then r.data_zaversheniya_remonta
end,
data_zaversheniya_remonta = case 
	when r.data_zaversheniya_remonta is not null then r.data_zaversheniya_remonta
	when r.data_nachala_remonta is not null then r.data_nachala_remonta
	when r.data_osmotra is not null then r.data_osmotra
end,
remontTypeID = 7,
r.remontCatID,
r.subdivisionID,
r.responsibleID,
r.primechanie,
r.shape,
transfer_flag = 1,
vid_rabotID = case (r.remontTypeID)
	when 1 then 4
	when 2 then 3
	when 3 then 2
end,
remontID = r2.id,
remontNodeID =  r.id
from remont r
join remont2 r2 on r2.otchet_po_defektu = r.otchet_po_defektu
order by r2.id
GO

update d 
	set 
	d.plan_flag = 1,
	utverdit = 1,
	data_nachala_plan = d.data_osmotra, 
	data_utverzhdeniya_plana = d.data_osmotra, 
	data_okonchaniya_plan = d.data_osmotra
from remont2 d
where d.remontTypeID in (1,2)
GO

update d 
	set 
	utverdit = 2
from remont2 d
where d.remontTypeID in (3)
GO


update d 
	set 
	d.plan_flag = 0,
	utverdit = 2,
	remontTypeID = 3,
	data_nachala_plan = d.data_osmotra, 
	data_utverzhdeniya_plana = d.data_osmotra, 
	data_okonchaniya_plan = d.data_osmotra
from remont2 d
where d.remontTypeID is null
GO

update d 
	set d.teplovaya_setID = t.teplovaya_set
from remont2 d
join (select t.id, t.teplovaya_set
from (
	select
		d.id,
		teplovaya_set = case 
							when pss.magistralSite is not null then 1 
							when pss.distSite is not null then 2
						end
	from remont2 d
	join remont2Deployed rd on rd.directionID = d.id
	JOIN heatPipeSections hpss ON hpss.lineID=rd.lineID
	JOIN pipeSections pss ON pss.id=hpss.pipeSectionID
)t
group by t.id, t.teplovaya_set ) t on t.id = d.id
GO

update d 
set d.show_map = 1
from defect d
where d.show_map is null
GO