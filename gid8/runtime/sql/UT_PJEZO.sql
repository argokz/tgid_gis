SELECT top 2147483647 
  l.id,
  ut_out.id AS id_oiut,
  ut_out.externalSignLineID,

  ec1.name as 'kod1', n1.externalNodeName as 'uzel1', 
  case ut_out.externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'П'
    when 5 then 'О'
  end as 'pr1',
  ec2.name as 'kod2', n2.externalNodeName as 'uzel2',
  case ut_out.externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'О'
    when 5 then 'П'
  end as 'pr2',
  sos, 
--  a7, a8, a9, a10, a11, a12, 
  
  a13, -- Расход сетевой воды на участке

  no1.pih AS 'Напор 1',
  no2.pih AS 'Напор 2',
  
  a14, a15, a16, a17,  -- Потери
  a10, -- Скорость
  a7, -- Длина
  a8, -- Вн.диаметр
  0 AS 'Расстояние', -- ??
  0 AS 'Объем от начального узла' -- ??

  
from UT_OUT
join linesobj l on l.id = ut_out.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
join US_OUT no1 on no1.nodeID = n1.id AND no1.calculationID = UT_OUT.calculationID AND (no1.externalSign=ut_out.externalSignLineID-1)
join US_OUT no2 on no2.nodeID = n2.id AND no2.calculationID = UT_OUT.calculationID AND (no2.externalSign=ut_out.externalSignLineID-1)


join externalCodes ec1 on ec1.id = n1.externalCodeID
join externalCodes ec2 on ec2.id = n2.externalCodeID
join externalSigns es1 on es1.id = n1.externalSignID
join externalSigns es2 on es2.id = n2.externalSignID
join externalSignLine esl on esl.id = ut_out.externalSignLineID
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on eci.id = ni.externalCodeID
left join externalSigns esi on esi.id = ni.externalSignID

WHERE UT_OUT.id in ($par$)
