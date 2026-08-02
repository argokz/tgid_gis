SELECT  
  n.id,
--  n.fileID,
--  consumerStateID,
  IIF(consumerStateID=1, ' ', 'закр') AS StateID,
  prizn,
  ec.name as kod,
--externalCodeID,    
--ec.name
uzel, 
name_building, 
otoplz, 
otopln, 
ventil, 

gvz, 

gvop, 
gvoo, 
rez, 

z, 
op, 
oo,
hs.name



FROM 
(




select
  nn.id,
  nn.fileID,
  rc.consumerStateID,
  ' ' AS prizn,

nn.externalCodeID,    
nn.externalNodeName  as  uzel, 
rc.name AS name_building, 
rc.calcHLdep as otoplz, 
rc.calcHLindep as otopln, 
rc.calcHLventil+rc.avgHLcond as ventil, 

rc.avgHLGVScloseParall+rc.avgHLGVScloseMix+rc.avgHLGVScloseConseq+rc.avgHLGVSclosePreon as gvz, 

rc.avgHLGVSopenFlow as gvop, 
rc.avgHLGVSopenRet as gvoo, 
rc.circHLosOpen as rez, 

rc.calcHLdep+rc.calcHLindep+rc.calcHLventil+rc.avgHLcond+
rc.avgHLGVScloseParall+rc.avgHLGVScloseMix+rc.avgHLGVScloseConseq+rc.avgHLGVSclosePreon as z, 
rc.avgHLGVSopenFlow as op, 
rc.avgHLGVSopenRet as oo


from
realConsumers rc
join nodes nn on nn.id = rc.nodeID
left join externalCodes ec on ec.id = nn.externalCodeID
left join externalSigns es on es.id = nn.externalSignID

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


UNION


select 
  nn.id,
  nn.fileID,
  rc.consumerStateID,
  'О' AS prizn,

nn.externalCodeID,    
nn.externalNodeName as  uzel, 
'' AS name_building, 
rc.calcHLdep+IIF (schemeParallID=3, 0, calcHLparall)+IIF (schemeMixID=3, 0, calcHLmix)+IIF (schemeConseqID=3, 0, calcHLconseq)+IIF (schemePreONID=3, 0, calcHLpreON) as otoplz, 
rc.calcHLindep+IIF (schemeParallID=3, calcHLparall, 0)+IIF (schemeMixID=3, calcHLmix, 0)+IIF (schemeConseqID=3, calcHLconseq, 0)+IIF (schemePreONID=3, calcHLpreON, 0) as otopln, 
rc.calcHLventil as ventil, 

rc.calcHLGVSparall+rc.calcHLGVSmix+rc.calcHLGVSconseq+rc.calcHLGVSpreON as gvz, 

rc.avgHLGVSopenSysFlow as gvop, 
rc.avgHLGVSopenSysRet as gvoo, 
rc.avgHLcompOpen as rez, 

rc.calcHLdep+calcHLparall+calcHLmix+calcHLconseq+calcHLpreON+
+rc.calcHLventil
+rc.calcHLGVSparall+rc.calcHLGVSmix+rc.calcHLGVSconseq+rc.calcHLGVSpreON as z, 
rc.avgHLGVSopenSysFlow as op, 
rc.avgHLGVSopenSysRet as oo


from
generalizedConsumers rc
join nodes nn on nn.id = rc.nodeID

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
) n

left join externalCodes ec on ec.id = n.externalCodeID
LEFT JOIN heatSources hs ON hs.id=ec.heatSourceID

WHERE n.fileID=$fileID$ AND 1=1

