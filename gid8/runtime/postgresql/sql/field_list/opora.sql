select 

CONCAT('Опора № ', obj.id, ' (', 
case priznak_truboprovoda
when 1 then 'общий'
when 2 then 'подающий'
when 3 then 'обратный'
end,
') ', ' назначение: ',  tip_opory.name)

from opora obj
join linesobj l on ST_Distance(l.shape, obj.shape) < 0.1
left join tip_opory on tip_opory.id=obj.vid_opory
where l.id=$fileID$



--select id_obj as 'id', CONCAT('Опора № ', id_obj,' ( ',ISNULL(externalLineStr,'признак трубопровада не указан'),' ), ','назначение: ',ISNULL(CAST(oporaType AS nvarchar(max)), 'не указано')) name
--from getPts_opora($fileID$,'pipe', '$FragmentIds$')
