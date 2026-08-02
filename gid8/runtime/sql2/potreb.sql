SELECT 
fr_id AS '#',
fr2_name AS 'Фрагмент',
sum(otopl_zco) AS 'Нагрузка на отопление зависимое, Гкал/ч',
sum(otopl_nco) AS 'Нагрузка на отопление независимое, Гкал/ч',
sum(otopl) AS 'Нагрузка на отопление общая, Гкал/ч',

sum(ventil_cv) AS 'Нагрузка на вентиляцию, Гкал/ч',
sum(kondiz) AS 'Нагрузка на кондиционирование, Гкал/ч',

sum(gv_pr) AS 'Нагрузка на гвс параллельное, Гкал/ч',
sum(gv_sm) AS 'Нагрузка на гвс смешанное, Гкал/ч',
sum(gv_ps) AS 'Нагрузка на гвс последовательное, Гкал/ч',
sum(gv_pw) AS 'Нагрузка на гвс предвключенное, Гкал/ч',
sum(gvz) AS 'Нагрузка на гвс закрытое, Гкал/ч',
sum(gv_op) AS 'Нагрузка на гвс открытое из подачи, Гкал/ч',
sum(gv_oo) AS 'Нагрузка на гвс открытое из обратки, Гкал/ч',

sum(q_ot_z) AS 'Расход на отопление, зависимое, т/ч',
sum(q_ot_n) AS 'Расход на отопление, независимое, т/ч',
sum(q_vent) AS 'Расход на вентиляцию, т/ч',
sum(q_kond) AS 'Расход на кондиционирование, т/ч',
sum(q_gv_z) AS 'Расход на гвс, закрытый, т/ч',
sum(q_gv_o_p) AS 'Расход на гвс, открытый из подачи, т/ч',
sum(q_gv_o_o) AS 'Расход на гвс, открытый из обратки, т/ч'


FROM (

SELECT 
n2.fileID AS fr_id,
fr2.name AS fr2_name,
gc.calcHLdep+IIF(gc.schemeMixID <> 3, gc.calcHLmix,0)+IIF(gc.schemeConseqID <> 3,gc.calcHLconseq,0)+IIF(gc.schemePreONID <> 3,gc.calcHLpreON,0)+IIF(gc.schemeParallID <> 3,gc.calcHLGVSparall,0) AS otopl_zco,
gc.calcHLindep+IIF(gc.schemeMixID = 3, gc.calcHLmix,0)+IIF(gc.schemeConseqID = 3,gc.calcHLconseq,0)+IIF(gc.schemePreONID = 3,gc.calcHLpreON,0)+IIF(gc.schemeParallID = 3,gc.calcHLGVSparall,0) AS otopl_nco,

gc.calcHLdep+gc.calcHLindep+gc.calcHLmix+gc.calcHLconseq+gc.calcHLpreON+gc.calcHLGVSparall AS otopl,

gc.calcHLventil AS ventil_cv,
gc.calcHLcond AS kondiz,

gc.calcHLGVSmix AS gv_sm,
gc.calcHLGVSconseq AS gv_ps,
gc.calcHLGVSpreON AS gv_pw,
gc.calcHLGVSparall AS gv_pr,

gc.calcHLGVSmix+gc.calcHLGVSconseq+gc.calcHLGVSpreON+gc.calcHLGVSparall AS gvz,
gc.avgHLGVSopenSysFlow AS gv_op,
gc.avgHLGVSopenSysRet AS gv_oo,

PT_OUT.a4 AS q_ot_z,
PT_OUT.a5 AS q_ot_n,
PT_OUT.a6 AS q_vent,
PT_OUT.a7 AS q_kond,
PT_OUT.a11 AS q_gv_z,
PT_OUT.a12 AS q_gv_o_p,
PT_OUT.a13 AS q_gv_o_o



FROM generalizedConsumers gc

JOIN nodes n ON n.id=gc.nodeID
JOIN externalCodes ec ON ec.id=n.externalCodeID

LEFT JOIN 

(
SELECT DISTINCT nodeID, kod_m, uzel_m, fragment_resultID  FROM setPressNodes
) sp


-- setPressNodes sp 


ON sp.kod_m=ec.name AND sp.uzel_m=n.externalNodeName AND sp.fragment_resultID = n.fileID
LEFT JOIN nodes n2 ON n2.id=sp.nodeID
LEFT JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID

LEFT JOIN fragments fr2 ON fr2.id=n2.fileID
LEFT JOIN PT_OUT ON PT_OUT.nodeID=n.id

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL AND n.removed=0 AND n2.removed=0 
AND calculationID=$calculationID$

) _TTTT

GROUP BY fr_id, fr2_name

