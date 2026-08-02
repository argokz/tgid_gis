select 

CONCAT('Колодец № ', obj.id, ' (', 
case priznak_truboprovoda
when 1 then 'общий'
when 2 then 'подающий'
when 3 then 'обратный'
end,
') ', ' диаметр: ',  obj.diametr)

from kolodtsy obj
join linesobj l on ST_Distance(l.shape, obj.shape) < 0.1
where l.id=$fileID$



--select id_obj as id, CONCAT('Колодец № ', id_obj,' ( ',ISNULL(externalLineStr,'признак трубопровада не указан'),' ), ','назначение: ',ISNULL(CAST(purposeTypes AS nvarchar(max)), 'не указано'))  name
--from getPts_kompensator_kolodtsy($fileID$,'pipe', '$FragmentIds$')
--where tblName = 'kolodtsy'