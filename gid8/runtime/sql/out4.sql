with wnutr as (

select 

n0.id AS n0id,
n.id AS nid,
n.externalSignID,

US_OUT.pih,
US_OUT.t,
UT_OUT.a13*IIF (l.nodeID1=cn.connectID AND l.nodeID2 = n0.id, -1, 1) AS r

from connectNodes cn

join nodes n ON n.id=cn.nodeID AND n.removed=0
join externalCodes ec ON ec.id=n.externalCodeID

JOIN
(
	SELECT 
	c.fileID,
	max(c.id) AS cid
	FROM CALCULATION c
	LEFT JOIN fragments fr ON fr.id=c.fileID
	GROUP BY c.fileID
) calc ON n.fileID=calc.fileID 

join nodes n0 ON n.internalNodeID=n0.id AND n0.removed=0
join externalCodes ec0 ON ec0.id=n0.externalCodeID
join US_OUT on US_OUT.nodeID=n.id AND US_OUT.calculationID=calc.cid
--8715 --calc.cid

join linesobj l on l.removed=0 AND (l.nodeID1=cn.connectID AND l.nodeID2 = n0.id) OR (l.nodeID2=cn.connectID AND l.nodeID1 = n0.id) 
AND (l.externalSignLineID = n.externalSignID OR l.externalSignLineID=1) AND l.removed=0

join UT_OUT on UT_OUT.lineID=l.id AND UT_OUT.externalSignLineID=n.externalSignID AND UT_OUT.calculationID=calc.cid
--=8715   --calc.cid

WHERE n0.fileID=$fileID$
)

,

wnutr2 as (
select
n0id,nid,
externalSignID,
IIF (externalSignID=2, pih, 0) AS pihP,
IIF (externalSignID=3, pih, 0) AS pihO,
IIF (externalSignID=2, t, 0) AS ttP,
IIF (externalSignID=3, t, 0) AS ttO,

IIF (externalSignID=2, r, 0) AS rP,
IIF (externalSignID=3, r, 0) AS rO


--*
from wnutr
)

/*
select 
n0id, nid,
sum(pihP) AS pihP,
sum(pihO) AS pihO,
sum(rP) AS rP,
sum(rO) AS rO

FROM wnutr2
group by n0id, nid


*/

,
wnutr3 as (
select 
ec0.name as kod0,
n0.externalNodeName as name0,
ec.name as kod,
n.externalNodeName as name,
n0id, nid, w.externalSignID, pihP, pihO, ttP, ttO, rP, rO

FROM wnutr2 w
JOIN nodes n0 ON n0.id=n0id
JOIN externalCodes ec0 ON ec0.id=n0.externalCodeID
JOIN nodes n ON n.id=nid
JOIN externalCodes ec ON ec.id=n.externalCodeID
)

,
wnutr4 as 
(

select 

row_number() over(partition by kod0, name0 order by kod0, name0, kod, name) as id1,
n0id,
kod0,
name0,
kod,
name,
sum(pihP) AS pihP,
sum(pihO) AS pihO,
sum(ttP) AS ttP,
sum(ttO) AS ttO,
sum(rP) AS rP,
sum(rO) AS rO

FROM wnutr3
GROUP BY
n0id,
kod0,
name0,
kod,
name
)

select 

row_number() over(order by n0id, id1) as id,

--id1,
--n0id AS id,
iif(id1=1, kod0, '') As Код,
iif(id1=1, name0, '') as Наименование,
kod As Код2,
name as Наименование2,
pihP,
pihO,
ttP AS Tп,
ttO AS Tо,
rP,
rO


from wnutr4
order by
    kod0,
    name0,
    id1,
    kod,
    name

