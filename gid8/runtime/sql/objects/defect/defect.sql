SELECT 
    t.id,
    l_id,
	t.beginNode AS 'Начальный узел',
	t.endNode AS 'Конечный узел',
	t.name AS 'Наименование объекта',
	t.defectType AS 'Тип дефекта',
	t.defectCat AS 'Категории нарушений',
	t.defectDescription AS 'Описание дефекта',
	t.defect_events AS 'Мероприятия для устранения дефекта',
	t.data_nachala_remonta AS 'Дата начала работ',
	t.data_zaversheniya_remonta AS 'Дата завершения работ',
	t.workListTube AS 'Характеристика/перечень работ на трубопроводе',
	t.workListChannel AS 'Характеристика/перечень работ в канале',
	t.workListKamera AS 'Характеристика/перечень работ в камере',
	t.len_tube AS 'Длина заменённой трубы, м',
	t.len_izol AS 'Длина заменённой изоляции, м',
	t.len_channel AS 'Длина участка ремонта канала,м',

	
    t.subdivision AS 'Подразделение производившее работы',
	t.dolzhnost AS 'Должность ответственного',
	t.fio_otv AS 'ФИО ответственного',

--	t.naimenovanie_uchastka AS '',
	t.fio AS 'Начальник участка'
--	t.primechanie AS '',
--	t.orderID AS '',
--	t.id as 'id_obj',
--	t.length AS '',
--	t.min_len AS '',
--	t.ms AS '',
--	t.rs AS ''
	FROM (
	  SELECT 
      l.id AS l_id,
	  --sd.name as 'state',
	  IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
	  IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
	  l.shape.STDistance(obj.shape) as length,
      MIN(l.shape.STDistance(obj.shape)) OVER(PARTITION BY obj.id ) AS "min_len",
	  srt.orderID,
	  obj.id AS id
	  ,obj.name 
	  ,deft.name as 'defectType'
	  ,obj.defectDescription
	  ,rc.name as 'defectCat'	  
      ,data_nachala_remonta
      ,data_zaversheniya_remonta
	  ,defect_events =
					STUFF(
					( SELECT CHAR(10) + rtt2.name AS n FROM defect r2 
					LEFT JOIN defectMeropr rt2 ON r2.id=rt2.objID
					LEFT JOIN defectMeroprType rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	  ,workListTube =
					STUFF(
					( SELECT CHAR(10) + rtt2.name AS n FROM defect r2 
					LEFT JOIN defectTube rt2 ON r2.id=rt2.objID
					LEFT JOIN remontTubeTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')

	  ,workListChannel =
					STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM defect r2 
					LEFT JOIN defectChannel rt2 ON r2.id=rt2.objID
					LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')	
	
	  ,workListKamera = 
					STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM defect r2 
					LEFT JOIN defectKamera rt2 ON r2.id=rt2.objID
					LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')	  	  
	  ,
	  obj.len_tube_cur AS len_tube,	  
	  obj.len_izol_cur AS len_izol,
	  obj.len_channel_cur AS len_channel,
	  obj.primechanie
	  ,subd.name as subdivision
	  ,d.znachenie as dolzhnost
      ,nach.fio as fio_otv
	  ,IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka
	  ,IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio
	  ,srt.uchastok_ms_id as 'ms'
	  ,srt.uchastok_rs_id as 'rs'
FROM defect obj
LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < 0.1
LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID	
LEFT JOIN nodes n1 ON n1.id = srt.nodeID1
LEFT JOIN nodes n2 ON n2.id = srt.nodeID2
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID
LEFT JOIN defectTypes deft ON deft.id = obj.remontTypeID
LEFT JOIN stateDefect sd ON sd.id = obj.stateID
LEFT JOIN remontCat rc ON rc.id = obj.remontCatID 
LEFT JOIN nachalniki_uchastkov nach ON nach.id=obj.responsibleID
LEFT JOIN dolzhnosti d ON d.id=nach.dolzhnost
LEFT JOIN subdivisions subd ON subd.id=subdivisionID
left join uchastok_ms ms ON ms.id = srt.uchastok_ms_id
left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
--left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
left join uchastok_rs rs ON rs.id = srt.uchastok_rs_id
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
--left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
$j$



WHERE 
l.removed=0 AND
n1.removed=0 AND 

NOT l.shape.STDistance(obj.shape) IS NULL
    	and n1.fileID in ($fileID$)
        AND ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) )
        $and_condition$
) t
$order$
