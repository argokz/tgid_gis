SELECT DISTINCT
n.id,
obj.name as 'Наименование/Адрес контура',
IIF (ms.id IS NOT NULL, ms.opisanie_uchastka_ms,rs.naimenovanie_uchastka_rs) AS 'Наименование тепловой сети МС/РС',
IIF (ms.id is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
IIF (n.NodeName IS NOT NULL, n.NodeName, n.externalNodeName) AS 'Наименование узла присоединения потребителя',
IIF (gc.id IS NOT NULL, gc.name, rc.name) AS 'Адрес',
IIF(gc.id is not NUll, 'Обобщенный', 'Реальный') as 'Вид потребителя',
obj.data_nachala_plan as 'Дата начала опрессовки',
obj.data_okonchaniya_plan as 'Дата окончания опрессовки',
nagr as 'Расчётная нагрузка, Гкал/ч',
--o.name AS 'Владелец',
--fr.name AS 'Фрагмент',
srt.orderID AS '#'

FROM nodes n
LEFT JOIN sortNodesForUchastok srt ON srt.nodeID=n.id
JOIN externalCodes ec ON ec.id=n.externalCodeID
JOIN fragments fr ON fr.id=n.fileID
LEFT JOIN generalizedConsumers gc ON gc.nodeID=n.id
LEFT JOIN realConsumers rc ON rc.nodeID=n.id
JOIN linesobj l ON l.nodeID1=n.id OR l.nodeID2=n.id
JOIN opresDeployed dep ON dep.lineID=l.id
JOIN opres obj ON obj.id=dep.directionID
LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN uchastok_rs rs ON rs.id=hps.distSite
LEFT JOIN uchastok_ms ms ON ms.id=hps.magistralSite
LEFT JOIN organizations o ON o.id=l.organizationID
left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii 
left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii

left join
(

SELECT 
id, 
fileID, 
hsid, 
otop+vent+gvz+gvop+gvoo AS nagr
FROM (
      SELECT 
      n.id,
      n.fileID,
      ec.heatSourceID AS hsid,
      calcHLdep+calcHLindep AS otop,
      calcHLventil AS vent,
      avgHLGVScloseParall+avgHLGVScloseMix+avgHLGVScloseConseq+avgHLGVSclosePreON AS gvz,
      contAvgHLGVSopenFlow AS gvop,
      contAvgHLGVSopenRet AS gvoo

      FROM realConsumers rs
      JOIN nodes n ON n.id=rs.nodeID
      JOIN externalCodes ec ON ec.id=n.externalCodeID
      WHERE rs.consumerStateID=1

    UNION

      SELECT
      n.id,
      n.fileID,
      ec.heatSourceID AS hsid,
      calcHLdep+calcHLindep+calcHLconseq+calcHLmix+calcHLparall+calcHLpreON AS otop,
      calcHLventil AS vent,
      calcHLGVSconseq+calcHLGVSmix+calcHLGVSparall+calcHLGVSpreON AS gvz,
      avgHLGVSopenSysFlow AS gvop,
      avgHLGVSopenSysRet AS gvoo

      FROM generalizedConsumers rs
      JOIN nodes n ON n.id=rs.nodeID
      JOIN externalCodes ec ON ec.id=n.externalCodeID
      WHERE rs.consumerStateID=1
)  _

) PT ON PT.id=n.id


WHERE n.removed=0 AND n.internalNodeID IS NULL
AND obj.id=$fileID$
--AND rs.id IS NULL AND (gc.id IS NOT NULL OR rc.id IS NOT NULL)
AND (gc.id IS NOT NULL OR rc.id IS NOT NULL)
ORDER BY srt.orderID
