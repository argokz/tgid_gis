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
  case IIF(ut_out.id IS NULL, l.externalSignLineID, ut_out.externalSignLineID)    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'П'    when 5 then 'О'  end as 'pr1',
  ec2.name as 'kod2', n2.externalNodeName as 'uzel2',
  case IIF(ut_out.id IS NULL, l.externalSignLineID, ut_out.externalSignLineID)    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'О'    when 5 then 'П'  end as 'pr2',
--  esl.name as 'externalSignLine',

hps.pipeSectLength as 'dlina',
hps.diameterInternal as 'diametr',
-- NULL AS a10,
-- NULL AS a11, 
-- NULL AS a12, 
-- NULL AS a13, 
-- NULL AS a14, 
-- NULL AS a16, 
-- NULL AS a15, 
-- NULL AS a17, 
-- NULL AS  a18, 
-- NULL AS  a19, 
-- NULL AS  a20, 
-- NULL AS  a21,

hs.sourceName



--  ,l.id
--  ,  eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p'
  
from linesobj l
left join UT_OUT on l.id = ut_out.lineID
join heatPipeSections hps ON hps.lineID=l.id
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


WHERE n1.fileID=$fileID$ AND (calculationID=$calculationID$ OR calculationID IS NULL)

AND n1.internalNodeID IS NULL

AND 
(
ut_out.id IS NULL

OR

(hps.pipeSectStateIDflow=2 AND (ut_out.externalSignLineID IN (1,2,4) OR ut_out.id IS NULL)
OR hps.pipeSectStateIDret=2 AND (ut_out.externalSignLineID IN (1,3,5) OR ut_out.id IS NULL)
)
)
AND l.removed=0
