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

sum(Qsum_z) AS 'Нагрузка на отопление общая, Гкал/ч',
sum(PT_OUT.a15) AS 'Расход на отопление, т/ч',

-- ------------------------------------------------------


sum(Qotz_treb)    AS 'Требуемая Нагрузка на отопление зависимое, Гкал/ч',  
sum(G_otoplz)     AS 'Требуемая Расход на отопление, зависимое, т/ч',         
sum(Qotn_treb)    AS 'Требуемая Нагрузка на отопление независимое, Гкал/ч',
sum(G_otopln)     AS 'Требуемая Расход на отопление, независимое, т/ч',       
sum(Qvent_treb)   AS 'Требуемая Нагрузка на вентиляцию и кондиционирование, Гкал/ч',        
sum(G_venkond)    AS 'Требуемая Расход на вентиляцию и кондиционирование, т/ч',                   
sum(Qgvz_treb)    AS 'Требуемая Нагрузка на гвс закрытое, Гкал/ч',            
sum(G_gvz)        AS 'Требуемая Расход на гвс, закрытый, т/ч',                
sum(isnull(Qotz_treb,0)+isnull(Qotn_treb,0)+isnull(Qvent_treb,0)+isnull(Qgvz_treb,0))   AS 'Требуемая Нагрузка на отопление общая, Гкал/ч',
sum(G_gz)         AS 'Требуемая Расход на отопление, т/ч',

-- ------------------------------------------------------


sum(otopl_zco) AS 'Расчетная Нагрузка на отопление зависимое, Гкал/ч',  
sum(G_otoplz)     AS 'Расчетная  Расход на отопление, зависимое, т/ч',         

sum(otopl_nco) AS 'Расчетная Нагрузка на отопление независимое, Гкал/ч',
sum(G_otopln)     AS 'Расчетная  Расход на отопление, независимое, т/ч',       


sum(ventil_cv+kondiz) AS 'Расчетная Нагрузка на вентиляцию и кондиционирование, Гкал/ч',        
sum(G_venkond)    AS 'Расчетная  Расход на вентиляцию и кондиционирование, т/ч',                   

sum(gvz)     AS 'Расчетная Нагрузка на гвс закрытое, Гкал/ч',            
sum(G_gvz)        AS 'Расчетная  Расход на гвс, закрытый, т/ч',                

sum(otopl) AS 'Расчетная Нагрузка на отопление общая, Гкал/ч',
sum(G_gz)         AS 'Расчетная  Расход на отопление, т/ч'




--2 AS num



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

calcExpendDep AS G_otoplz,
calcExpendIndep AS G_otopln,
calcExpendVentil AS G_venkond,
calcExpendHWparall+calcExpendHWmix+calcExpendHWconseq+calcExpendHWpreON AS G_gvz,
calcExpendDep+calcExpendIndep+calcExpendVentil+calcExpendCond+calcExpendHWparall+calcExpendHWmix+calcExpendHWconseq+calcExpendHWpreON AS G_gz


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


calcExpendDep AS G_otoplz,
calcExpendIndep AS G_otopln,
calcExpendVentil AS G_venkond,
calcExpendHWparall+calcExpendHWmix+calcExpendHWconseq+calcExpendHWpreON AS G_gvz,
calcExpendDep+calcExpendIndep+calcExpendVentil+calcExpendCond+calcExpendHWparall+calcExpendHWmix+calcExpendHWconseq+calcExpendHWpreON AS G_gz

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
