with tubes1 AS (

SELECT
n1.fileID,

l.id,
hps.id AS hps_id,
hps.pipeSectionID,
ps1.nodeID1,
ps1.nodeID2,

l.externalSignLineID,

hps.magistralSite,
hps.distSite,

max(IIF (l.externalSignLineID <> 3, hps.diameterExternal, 0)) over(partition by ps1.id) AS diamP,
max(IIF (l.externalSignLineID <> 2, hps.diameterExternal, 0)) over(partition by ps1.id) AS diamO,

max(IIF (l.externalSignLineID <> 3, hps.wallThickness, 0)) over(partition by ps1.id) AS tolP,
max(IIF (l.externalSignLineID <> 2, hps.wallThickness, 0)) over(partition by ps1.id) AS tolO,


IIF (l.externalSignLineID <> 3, hps.pipeSectLength, 0) AS lenP, 
IIF (l.externalSignLineID <> 2, hps.pipeSectLength, 0) AS lenO, 

sum(IIF (l.externalSignLineID <> 3, hps.pipeSectLength, 0)) over(partition by ps1.id) AS lenP2, 
sum(IIF (l.externalSignLineID <> 2, hps.pipeSectLength, 0)) over(partition by ps1.id) AS lenO2, 



IIF (l.externalSignLineID <> 3, hps.pipeSectLength*POWER(hps.diameterInternal/1000, 2)*PI()/4, 0) AS vP, 
IIF (l.externalSignLineID <> 2, hps.pipeSectLength*POWER(hps.diameterInternal/1000, 2)*PI()/4, 0) AS vO, 

sum(IIF (l.externalSignLineID <> 3, hps.pipeSectLength*POWER(hps.diameterInternal/1000, 2)*PI()/4, 0)) over(partition by ps1.id)  AS vP2, 
sum(IIF (l.externalSignLineID <> 2, hps.pipeSectLength*POWER(hps.diameterInternal/1000, 2)*PI()/4, 0)) over(partition by ps1.id)  AS vO2, 



COUNT(*) over(partition by ps1.id) AS cnt,
ROW_NUMBER() over(partition by ps1.id order by hps.pipeSectLength DESC) AS rank1,
hps.tubeTypeID,
hps.tubingTypeID,
max(srt.orderID) over (partition by ps1.id) as orderID


FROM linesobj l
JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN pipeSections ps1 ON ps1.id=hps.pipeSectionID
left join sortLinesForUchastok srt on srt.pipeSectionID = ps1.id
JOIN nodes n1 ON n1.id=l.nodeID1
JOIN nodes n2 ON n2.id=l.nodeID2

WHERE l.removed=0 AND n1.internalNodeID IS NULL AND n1.fileID=58
AND ps1.id IS NOT NULL

)

,
tubes AS (
SELECT 
* 
FROM tubes1
WHERE rank1=1


UNION ALL
SELECT 

fileID,

NULL AS id,
NULL AS hps_id,
-1 AS pipeSectionID,
NULL AS nodeID1,
NULL AS nodeID2,
NULL,

magistralSite,
distSite,

diamP,
diamO,

--0 AS wallThickness,

NULL AS tolP,
NULL AS tolO,


NULL AS lenP, 
NULL AS lenO, 

sum(lenP) AS lenP2, 
sum(lenO) AS lenO2, 

NULL AS vP, 
NULL AS vO, 

sum(vP) AS vP2, 
sum(vO) AS vO2, 


NULL AS cnt,
NULL AS rank1,
NULL AS tubeTypeID,
NULL AS tubingTypeID,
0 AS orderID


FROM tubes1
--WHERE distSite=62

GROUP BY fileID, 
magistralSite,
distSite,
diamP,
diamO


UNION ALL
SELECT 

fileID,

NULL AS id,
NULL AS hps_id,
-2 AS pipeSectionID,
NULL AS nodeID1,
NULL AS nodeID2,
NULL,

magistralSite,
distSite,

NULL AS diamP,
NULL AS diamO,

--0 AS wallThickness,

NULL AS tolP,
NULL AS tolO,


--0 AS pipeSectLength,

NULL AS lenP, 
NULL AS lenO, 

sum(lenP) AS lenP2, 
sum(lenO) AS lenO2, 



NULL AS vP, 
NULL AS vO, 

sum(vP) AS vP2, 
sum(vO) AS vO2, 


0 AS cnt,
0 AS rank1,

0 AS tubeTypeID,
0 AS tubingTypeID,
0 AS orderID


FROM tubes1
--WHERE distSite=62

GROUP BY fileID, 
magistralSite,
distSite





)


SELECT 

t.fileID,
t.magistralSite,
t.distSite,

IIF (t.pipeSectionID = -1, 'Итого по диаметру', 
IIF (t.pipeSectionID = -2, 'Итого', 

IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ', n1.externalNodeName, n1.nodeName) 
)
)
as 'beginPts'


,



IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ', n2.externalNodeName, n2.nodeName) as 'rndPts',
t.diamP,
t.lenP2,
t.diamO,
t.lenO2,
t.tolP,
t.tolO,
t.vP2,
t.vO2,
typeT.name,
viewT.name
--t.orderID

--*


FROM tubes t
LEFT JOIN nodes n1 ON n1.id=t.nodeID1
LEFT JOIN nodes n2 ON n2.id=t.nodeID2
left join tubeTypes viewT ON viewT.id = t.tubeTypeID
left join tubingTypes typeT ON typeT.id = t.tubingTypeID

WHERE

1=1
--t.distSite=62
--AND rank1 <= 1

--AND n1.nodeName='УТ-50(4)'
AND t.magistralSite=9

ORDER BY
t.fileID, 
t.magistralSite,
t.distSite,

CASE
        WHEN t.pipeSectionID = -2 THEN 2
        WHEN t.pipeSectionID = -1 THEN 1
        ELSE 0
END,

t.pipeSectionID,
t.orderID,

t.diamP,
t.diamO





