SELECT 
n.externalCodeID AS '#',
-- hs.name AS name_src,
ec.name AS 'Расчетная схема',
hs.name AS name_src,

sum(Qotz) AS 'Нагрузка на отопление зависимое, Гкал/ч',  
sum(PT_OUT.a4)  AS 'Расход на отопление, зависимое, т/ч',         

sum(Qotn) AS 'Нагрузка на отопление независимое, Гкал/ч',
sum(PT_OUT.a5)  AS 'Расход на отопление, независимое, т/ч',       


sum(dop12+dop13) AS 'Нагрузка на вентиляцию и кондиционирование, Гкал/ч',        
sum(PT_OUT.a6)  AS 'Расход на вентиляцию и кондиционирование, т/ч',                   

sum(dop17)     AS 'Нагрузка на гвс закрытое, Гкал/ч',            
sum(PT_OUT.a11) AS 'Расход на гвс, закрытый, т/ч',                


sum(dop18)   AS 'Нагрузка на гвс открытое из подачи, Гкал/ч',  
sum(PT_OUT.a12) AS 'Расход на гвс, открытый из подачи, т/ч',


sum(dop19)   AS 'Нагрузка на гвс открытое из обратки, Гкал/ч', 
sum(PT_OUT.a13) AS 'Расход на гвс, открытый из обратки, т/ч',

sum(dop20)   AS 'Нагрузка на гвс рециркуляцию, Гкал/ч', 
sum(PT_OUT.a14) AS 'Расход на рециркуляцию, т/ч',


sum(Qsum_z) AS 'Нагрузка на отопление общая, Гкал/ч',
sum(PT_OUT.a15) AS 'Расход на отопление, т/ч',

sum(dop22)   AS 'Нагрузка открытое из подачи, Гкал/ч',  
sum(PT_OUT.a12) AS 'Расход открытый из подачи, т/ч',

sum(dop23)   AS 'Нагрузка открытое из обратки, Гкал/ч', 
sum(PT_OUT.a13) AS 'Расход  открытый из обратки, т/ч'



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
rc.avgHLGVSopenRet AS gv_oo
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
gc.avgHLGVSopenSysRet AS gv_oo
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
