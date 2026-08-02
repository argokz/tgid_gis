SELECT top 2147483647 
  l.id,
  IIF(
    (ut_out.id IS NULL OR ut_out.externalSignLineID IN (1,2,4)) AND hps.pipeSectStateIDflow=2 AND
    (ut_out.id IS NULL OR ut_out.externalSignLineID IN (1,3,5)) AND hps.pipeSectStateIDret=2,
     
     'закр', ''
  ) AS sost,

--  eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p',
  ec1.name as 'kod1', n1.externalNodeName as 'uzel1', 
--  ut_out.externalSignLineID,
  case ut_out.externalSignLineID    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'П'    when 5 then 'О'  end as 'pr1',
  ec2.name as 'kod2', n2.externalNodeName as 'uzel2',
  case ut_out.externalSignLineID    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'О'    when 5 then 'П'  end as 'pr2',
--  esl.name as 'externalSignLine',
--  a7, a8, 

hps.pipeSectLength as 'dlina',
hps.diameterInternal as 'diametr',


  a10, 
  a11, a12, 
  a13, a14, a16, a15, a17, 
  a18, a19, a20, a21,
  hs.sourceName
--  ,l.id

--  ,  eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p'
  
from linesobj l
join heatPipeSections hps ON hps.lineID=l.id
left join UT_OUT on l.id = ut_out.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
join externalCodes ec1 on ec1.id = n1.externalCodeID
join externalCodes ec2 on ec2.id = n2.externalCodeID
join externalSigns es1 on es1.id = n1.externalSignID
join externalSigns es2 on es2.id = n2.externalSignID
left join externalSignLine esl on esl.id = ut_out.externalSignLineID
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on eci.id = ni.externalCodeID
left join externalSigns esi on esi.id = ni.externalSignID
LEFT JOIN heatSources hs ON hs.id = ec1.heatSourceID

WHERE n1.fileID=$fileID$ AND n1.internalNodeID IS NULL AND (calculationID=$calculationID$ OR calculationID IS NULL)
AND l.removed=0
