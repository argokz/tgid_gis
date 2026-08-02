SELECT

id,
StateID,
obob,
kod,uzel, 
name_building, 
otopl, 
ventil, 
gvz, 
calcInternHD,

gvop, 
gvoo, 
rez, 

z, 
op, 
oo, 

dog_otop,

dog_vent,
dog_gvs,
dog_teplovyd,
dog_gv_pod,
dog_gv_obr,
dog_gv_rz,


z2,

dog_gv_pod2,
dog_gv_obr2


 
FROM (

SELECT 
nn.id,
nn.fileID,
IIF(rc.consumerStateID=1, ' ', 'закр') AS StateID,
'O' AS obob,
ec.name  as 'kod',    nn.externalNodeName  as  'uzel', 
'' AS name_building, 
rc.calcHLdep+rc.calcHLindep
+calcHLparall+calcHLmix+calcHLconseq+calcHLpreON
 as 'otopl', 
rc.calcHLventil as 'ventil', 

rc.calcHLGVSparall+rc.calcHLGVSmix+rc.calcHLGVSconseq+rc.calcHLGVSpreON as 'gvz', 

0 AS calcInternHD,

rc.avgHLGVSopenSysFlow as 'gvop', 
rc.avgHLGVSopenSysRet as 'gvoo', 
rc.avgHLcompOpen as 'rez', 

rc.calcHLdep+rc.calcHLindep+rc.calcHLventil+
rc.calcHLGVSparall+rc.calcHLGVSmix+rc.calcHLGVSconseq+rc.calcHLGVSpreON as 'z', 
rc.avgHLGVSopenSysFlow as 'op', 
rc.avgHLGVSopenSysRet as 'oo', 


ISNULL(adjCalcHLdep, 0)+ISNULL(adjCalcHLindep, 0)+ISNULL(adjCalcHLparall, 0)+ISNULL(adjCalcHLmix, 0)+ISNULL(adjCalcHLpreON, 0)+ISNULL(adjCalcHLconseq, 0) AS dog_otop,

ISNULL(adjCalcHLventil, 0)+ISNULL(adjCalcHLcond, 0) AS dog_vent,
ISNULL(adjCalcHLGVSparall, 0)+ISNULL(adjCalcHLGVSmix, 0)+ISNULL(adjCalcHLGVSpreON, 0)+ISNULL(adjCalcHLGVSconseq, 0) AS dog_gvs,
ISNULL(adjCalcHDdep, 0)+ISNULL(adjCalcInternHDindep, 0)+ISNULL(adjInternHDparall, 0)+ISNULL(adjInternHDmix, 0)+ISNULL(adjInternHDconseq, 0)+ISNULL(adjInternHDpreON, 0) AS dog_teplovyd,
ISNULL(adjAvgHLGVSopenSysFlow, 0) AS dog_gv_pod,
ISNULL(adjAvgHLGVSopenSysRet, 0)  AS dog_gv_obr,
0 AS dog_gv_rz,


ISNULL(adjCalcHLdep, 0)+ISNULL(adjCalcHLindep, 0)+ISNULL(adjCalcHLparall, 0)+ISNULL(adjCalcHLmix, 0)+ISNULL(adjCalcHLpreON, 0)+ISNULL(adjCalcHLconseq, 0)+

ISNULL(adjCalcHLventil, 0)+ISNULL(adjCalcHLcond, 0)+
ISNULL(adjCalcHLGVSparall, 0)+ISNULL(adjCalcHLGVSmix, 0)+ISNULL(adjCalcHLGVSpreON, 0)+ISNULL(adjCalcHLGVSconseq, 0)
- (ISNULL(adjCalcHDdep, 0)+ISNULL(adjCalcInternHDindep, 0)+ISNULL(adjInternHDparall, 0)+ISNULL(adjInternHDmix, 0)+ISNULL(adjInternHDconseq, 0)+ISNULL(adjInternHDpreON, 0))
 AS 'z2',

ISNULL(adjAvgHLGVSopenSysFlow, 0) AS dog_gv_pod2,
ISNULL(adjAvgHLGVSopenSysRet, 0)  AS dog_gv_obr2




--hs.name,
--org.name as 'kod_owner'

from generalizedConsumers rc
join nodes nn on nn.id = rc.nodeID
left join externalCodes ec on ec.id = nn.externalCodeID


JOIN
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON nn.fileID=calc.fileID



WHERE nn.removed=0

AND 

(
NOT nn.id
IN (

SELECT nn.id

FROM generalizedConsumers rc
JOIN nodes nn ON nn.id=rc.nodeID

JOIN
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON nn.fileID=calc.fileID
JOIN PT_OUT ON PT_OUT.calculationID=cid AND PT_OUT.nodeID=nn.id

)
OR rc.consumerStateID=2
) 

UNION

select
nn.id,
nn.fileID,
' ' AS obob,
IIF(rc.consumerStateID=1, ' ', 'закр') AS StateID,
ec.name  as 'kod',    nn.externalNodeName  as  'uzel', 
rc.name AS name_building, 

rc.calcHLdep+rc.calcHLindep as 'otopl', 
rc.calcHLventil+rc.avgHLcond as 'ventil', 

rc.avgHLGVScloseParall+rc.avgHLGVScloseMix+rc.avgHLGVScloseConseq+rc.avgHLGVSclosePreon as 'gvz', 

calcInternHD,

rc.avgHLGVSopenFlow as 'gvop', 
rc.avgHLGVSopenRet as 'gvoo', 
rc.circHLosOpen as 'rez', 

rc.calcHLdep+rc.calcHLindep+rc.calcHLventil+rc.avgHLcond+
rc.avgHLGVScloseParall+rc.avgHLGVScloseMix+rc.avgHLGVScloseConseq+rc.avgHLGVSclosePreon as 'z', 
rc.avgHLGVSopenFlow as 'op', 
rc.avgHLGVSopenRet as 'oo', 

  contCalcHLdep+contcalcHLindep AS dog_otop,
  contCalcHLventil AS dog_vent,
  contAvgHLGVScloseParall+contAvgHLGVScloseMix+contAvgHLGVScloseConseq+contAvgHLGVSclosePreON AS dog_gvs,
  contInternHD AS dog_teplovyd,
  contAvgHLGVSopenFlow AS dog_gv_pod,
  contAvgHLGVSopenRet AS dog_gv_obr,
  0 AS dog_gv_rz,


  contCalcHLdep+contcalcHLindep+ contCalcHLventil+
  contAvgHLGVScloseParall+contAvgHLGVScloseMix+contAvgHLGVScloseConseq+contAvgHLGVSclosePreON
  -contInternHD AS z2,



  contAvgHLGVSopenFlow AS dog_gv_pod2,
  contAvgHLGVSopenRet AS dog_gv_obr2




--hs.name,
--org.name as 'kod_owner'

from
realConsumers rc
join nodes nn on nn.id = rc.nodeID
left join externalCodes ec on ec.id = nn.externalCodeID


JOIN
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON nn.fileID=calc.fileID


WHERE nn.removed=0

AND 

(
NOT nn.id
IN (


SELECT nn.id

FROM realConsumers rc
JOIN nodes nn ON nn.id=rc.nodeID

JOIN
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON nn.fileID=calc.fileID
JOIN PT_OUT ON PT_OUT.calculationID=cid AND PT_OUT.nodeID=nn.id

)
OR rc.consumerStateID=2
)
) n

WHERE n.fileID=$fileID$ AND NOT (otopl=0 AND ventil=0 AND gvz=0 AND gvop=0 AND gvoo=0)
