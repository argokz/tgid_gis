select 

CONCAT('Компенсатор № ', obj.id, ' (', 
case priznak_truboprovoda
when 1 then 'общий'
when 2 then 'подающий'
when 3 then 'обратный'
end,
') ', ' диаметр: ',  obj.diametr_truboprovoda)

from kompensator obj
join linesobj l on ST_Distance(l.shape, obj.shape) < 0.1
where l.id=$fileID$


--select id_obj as id, CONCAT('Компенсатор № ', id_obj,' ( ',ISNULL(externalLineStr,'признак трубопровада не указан'),' ), ','конструкция: ',ISNULL(CAST(constructionTypes AS nvarchar(max)), 'не указана')) name
--from getPts_kompensator_kolodtsy($fileID$,'pipe', '$FragmentIds$')
--where tblName = 'kompensator'
