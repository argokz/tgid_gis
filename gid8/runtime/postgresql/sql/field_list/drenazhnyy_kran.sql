select 

CONCAT('Дренажный кран № ', obj.id, ' (', 
case priznak_truboprovoda
when 1 then 'общий'
when 2 then 'подающий'
when 3 then 'обратный'
end,
') ', ' диаметр: ',  obj.diametr)

from drenazhnyy_kran obj
join linesobj l on ST_Distance(l.shape, obj.shape) < 0.1
where l.id=$fileID$
