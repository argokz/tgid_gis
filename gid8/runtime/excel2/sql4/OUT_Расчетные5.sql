SELECT 
n.externalCodeID AS '#',
-- hs.name AS name_src,
ec.name AS 'Расчетная схема',
hs.name AS name_src,

  sum(dop18) AS aa0,
  sum(a12) AS aa1,
  sum(dop19) AS aa2,
  sum(a13) AS aa3,
  sum(dop20) AS aa4,
  sum(a14) AS aa5,

  sum(Qgvop_treb) AS aa6,
  sum(G_gvop) AS aa7,
  sum(Qgvoo_treb) AS aa8,
  sum(G_gvoo) AS aa9,
  sum(Qrez_treb) AS aa10,
  sum(G_rez) AS aa11,

  sum(gv_op) AS aa12,
  sum(G_gvop) AS aa13,
  sum(gv_oo) AS aa14,
  sum(G_gvoo) AS aa15,
  sum(rez_q) AS aa16,
  sum(G_rez) AS aa17



FROM 
(
SELECT 
nodeID,
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

calcExpendHWFlow AS G_gvop,
calcExpendHWRet  AS G_gvoo,
calcExpendRecircOpen AS G_rez,
(rc.avgHLGVSopenFlow+rc.avgHLGVSopenRet)*circHLosOpen/100 AS rez_q



FROM realConsumers rc
WHERE rc.consumerStateID=1

UNION

SELECT 
nodeID,
gc.consumerStateID,
gc.calcHLdep+IIF(gc.schemeMixID <> 3, gc.calcHLmix,0)+IIF(gc.schemeConseqID <> 3,gc.calcHLconseq,0)+IIF(gc.schemePreONID <> 3,gc.calcHLpreON,0)+IIF(gc.schemeParallID <> 3,gc.calcHLGVSparall,0) AS otopl_zco,
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


calcExpendHWFlow AS G_gvop,
calcExpendHWRet  AS G_gvoo,
calcExpendRecircOpen AS G_rez,
(avgHLGVSopenSysFlow+avgHLGVSopenSysRet)*avgHLcompOpen/100 AS rez_q


FROM generalizedConsumers gc



) potr

JOIN nodes n ON n.id=potr.nodeID AND n.removed=0
JOIN externalCodes ec ON ec.id=n.externalCodeID
JOIN PT_OUT ON PT_OUT.nodeID=n.id
JOIN heatSources hs ON hs.id=ec.heatSourceID

JOIN
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON PT_OUT.calculationID=calc.cid

WHERE n.fileID = $fileID$ AND potr.consumerStateID=1

GROUP BY n.externalCodeID, ec.name, hs.name
-- ORDER BY n.externalCodeID
