SELECT 
n.externalCodeID AS '#',
ec.name AS 'Расчетная схема',
sum(otopl_zco) AS 'Нагрузка на отопление зависимое, Гкал/ч',  
sum(otopl_nco) AS 'Нагрузка на отопление независимое, Гкал/ч',

sum(otopl) AS 'Нагрузка на отопление общая, Гкал/ч',
sum(otopl_tp) AS 'Внутренние тепловыделения, ГВС/ч',

sum(ventil_cv) AS 'Нагрузка на вентиляцию, Гкал/ч',        
sum(kondiz)    AS 'Нагрузка на кондиционирование, Гкал/ч', 

sum(gv_pr) AS 'Нагрузка на гвс параллельное, Гкал/ч',     
sum(gv_sm) AS 'Нагрузка на гвс смешанное, Гкал/ч',        
sum(gv_ps) AS 'Нагрузка на гвс последовательное, Гкал/ч', 
sum(gv_pw) AS 'Нагрузка на гвс предвключенное, Гкал/ч',   

sum(gvz)     AS 'Нагрузка на гвс закрытое, Гкал/ч',            
sum(gv_op)   AS 'Нагрузка на гвс открытое из подачи, Гкал/ч',  
sum(gv_oo)   AS 'Нагрузка на гвс открытое из обратки, Гкал/ч', 
sum(PT_OUT.a4)  AS 'Расход на отопление, зависимое, т/ч',         
sum(PT_OUT.a5)  AS 'Расход на отопление, независимое, т/ч',       
sum(PT_OUT.a6)  AS 'Расход на вентиляцию, т/ч',                   
sum(PT_OUT.a7)  AS 'Расход на кондиционирование, т/ч',            
sum(PT_OUT.a11) AS 'Расход на гвс, закрытый, т/ч',                
sum(PT_OUT.a12) AS 'Расход на гвс, открытый из подачи, т/ч',
sum(PT_OUT.a13) AS 'Расход на гвс, открытый из обратки, т/ч',
sum(PT_OUT.a14) AS 'Расход на рециркуляцию, т/ч'




FROM 
(
SELECT 
nodeID,
rc.consumerStateID,
rc.calcInternHD AS 'otopl_tp', 
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
gc.calcInternHDdep+gc.calcInternHDindep+gc.internHDparall+gc.internHDmix+gc.internHDconseq+gc.internHDpreON AS 'otopl_tp', 
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

JOIN
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON PT_OUT.calculationID=calc.cid

WHERE n.fileID=$fileID$ AND potr.consumerStateID=1

GROUP BY n.externalCodeID, ec.name
ORDER BY n.externalCodeID
