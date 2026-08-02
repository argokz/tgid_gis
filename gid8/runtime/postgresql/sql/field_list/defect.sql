select 

CONCAT('Дефект № ', obj.id, ' (', 
case priznak_truboprovoda
when 1 then 'общий'
when 2 then 'подающий'
when 3 then 'обратный'
end,
') ', ' назначение: ',  tip_opory.name)

from defect obj
join linesobj l on ST_Distance(l.shape, obj.shape) < 0.1
left join tip_opory on tip_opory.id=obj.vid_opory
where l.id=$fileID$

--select distinct 
--    id_obj as 'id', 
--    CONCAT('Дефект № ', id_obj,' ( ', 
--    ISNULL(CAST([Дата обнаружения нарушения] as nvarchar(max)) , '-'  ),' ) '  
--    , ', Категория нарушений: ', ISNULL([Категория нарушения],'не указано' ) )  name
--from  getPts_defect($fileID$,'pipe', '$FragmentIds$')