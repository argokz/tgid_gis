SELECT
--n.id,
StateID,
obob AS po_pr,
ec.name AS kod,
n.externalNodeName AS uzel_name,
potr.name,

  iif(Qotz is NULL, 0, Qotz)+iif(Qotn is NULL, 0, Qotn) AS Qot, 
  iif(dop12 is NULL, 0, dop12)+iif(dop13 is NULL, 0, dop13) AS Qvent, 
  dop17, 
  dop18, 
  dop19, 
  dop20, 
  Qsum_z, 
  dop18 AS dop18_2, 
  dop19 AS dop19_2



FROM 
(
SELECT 
IIF(rc.consumerStateID=1, ' ', 'закр') AS StateID,
' ' AS obob,
nodeID,
rc.name,
rc.consumerStateID,

rc.calcHLdep AS otopl_zco,
rc.calcHLindep AS otopl_nco,
rc.calcHLdep+rc.calcHLindep AS otopl,
rc.calcHLventil AS ventil_cv,
0 AS kondiz,
rc.avgHLGVScloseMix AS gv_sm,
rc.avgHLGVScloseConseq AS gv_ps,
rc.avgHLGVSclosePreON AS gv_pw,
rc.avgHLGVScloseParall AS gv_pr,

rc.avgHLGVScloseMix+rc.avgHLGVScloseConseq+rc.avgHLGVSclosePreON+rc.avgHLGVScloseParall AS gvz,
rc.avgHLGVSopenFlow AS gv_op,
rc.avgHLGVSopenRet AS gv_oo,
calcInternHD AS otopl_tp,

  contCalcHLdep+contcalcHLindep AS dog_otop,
  contCalcHLventil AS dog_vent,
  contAvgHLGVScloseParall+contAvgHLGVScloseMix+contAvgHLGVScloseConseq+contAvgHLGVSclosePreON AS dog_gvs,
  contInternHD AS dog_teplovyd,
  contAvgHLGVSopenFlow AS dog_gv_pod,
  contAvgHLGVSopenRet AS dog_gv_obr,
  0 AS dog_gv_rz



FROM realConsumers rc
--WHERE rc.consumerStateID=1

UNION

SELECT 
IIF(gc.consumerStateID=1, ' ', 'закр') AS StateID,
'О' AS obob,
nodeID,
'' AS name,
gc.consumerStateID,

gc.calcHLdep+IIF(gc.schemeMixID = 3, 0, gc.calcHLmix)+IIF(gc.schemeConseqID = 3,0, gc.calcHLconseq)+IIF(gc.schemePreONID = 3,0,gc.calcHLpreON)+IIF(gc.schemeParallID = 3,0, gc.calcHLGVSparall) AS otopl_zco,
gc.calcHLindep+IIF(gc.schemeMixID = 3, gc.calcHLmix,0)+IIF(gc.schemeConseqID = 3,gc.calcHLconseq,0)+IIF(gc.schemePreONID = 3,gc.calcHLpreON,0)+IIF(gc.schemeParallID = 3,gc.calcHLGVSparall,0) AS otopl_nco,
gc.calcHLdep+gc.calcHLindep+gc.calcHLmix+gc.calcHLconseq+gc.calcHLpreON+gc.calcHLGVSparall AS otopl,
gc.calcHLventil AS ventil_cv,
ISNULL(gc.calcHLcond, 0) AS kondiz,
gc.calcHLGVSmix AS gv_sm,
gc.calcHLGVSconseq AS gv_ps,
gc.calcHLGVSpreON AS gv_pw,
gc.calcHLGVSparall AS gv_pr,
gc.calcHLGVSmix+gc.calcHLGVSconseq+gc.calcHLGVSpreON+gc.calcHLGVSparall AS gvz,
gc.avgHLGVSopenSysFlow AS gv_op,
gc.avgHLGVSopenSysRet AS gv_oo,
calcInternHDdep+calcInternHDindep+internHDparall+internHDconseq+internHDmix+internHDpreON AS otopl_tp,



adjCalcHLdep+adjCalcHLindep+adjCalcHLparall+adjCalcHLmix+adjCalcHLpreON+adjCalcHLconseq AS dog_otop,

adjCalcHLventil+adjCalcHLcond AS dog_vent,
adjCalcHLGVSparall+adjCalcHLGVSmix+adjCalcHLGVSpreON+adjCalcHLGVSconseq AS dog_gvs,
adjCalcHDdep+adjCalcInternHDindep+adjInternHDparall+adjInternHDmix+adjInternHDconseq+adjInternHDpreON AS dog_teplovyd,
adjAvgHLGVSopenSysFlow AS dog_gv_pod,
adjAvgHLGVSopenSysRet  AS dog_gv_obr,
0 AS dog_gv_rz


FROM generalizedConsumers gc

) potr

JOIN nodes n ON n.id=potr.nodeID AND n.removed=0
JOIN externalCodes ec ON ec.id=n.externalCodeID
JOIN PT_OUT ON PT_OUT.nodeID=n.id

JOIN
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON PT_OUT.calculationID=calc.cid

WHERE n.fileID=$fileID$ AND n.removed=0
