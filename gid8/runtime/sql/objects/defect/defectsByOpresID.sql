select 
	distinct
    d.id,
	IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Начальный узел',
	IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Конечный узел',	
	--d.name as 'nameObj',
	deft.name as 'Источник повреждения',
	d.defectDescription as 'Описание повреждения',
	rc.name as 'Категория нарушения',
	sd.name as 'Состояние',
    d.data_nachala_remonta as 'Дата начала ремонта',
    d.data_zaversheniya_remonta as 'Дата завершения ремонта',
	'Мероприятия' =
					STUFF(
					( SELECT CHAR(10) + rtt2.name AS n FROM defect r2 
					LEFT JOIN defectMeropr rt2 ON r2.id=rt2.objID
					LEFT JOIN defectMeroprType rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=d.id and r2.remontTypeID = 2 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Перечень работ (трубопроводы)' =
					STUFF(
					( SELECT CHAR(10) + rtt2.name AS n FROM defect r2 
					LEFT JOIN defectTube rt2 ON r2.id=rt2.objID
					LEFT JOIN remontTubeTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=d.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')

	  ,'Перечень работ (канал)' =
					STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM defect r2 
					LEFT JOIN defectChannel rt2 ON r2.id=rt2.objID
					LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=d.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	
	  ,'Перечень работ (камеры)' = 
					STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM defect r2 
					LEFT JOIN defectKamera rt2 ON r2.id=rt2.objID
					LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=d.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	  ,
	d.otchet_po_defektu AS 'Отчет по дефекту',
	d.len_tube_cur AS 'Длина заменённой трубы, м',	  
	d.len_izol_cur AS 'Длина заменённой изоляции, м',
	d.len_channel_cur AS 'Длина участка ремонта канала, м',
	d.primechanie AS 'Примечание',
	d.data_osmotra AS 'Дата осмотра',
	d.vremya_osmotra AS 'Время осмотра',
--    ,
--		obj.name,
--		obj.opisaniye_kontura,
		vid_is.name as 'Вид испытания',
		obj.date_opres as 'Дата испытания',
--		obj.otchet,
--		obj.primechanie,
		subd.name  as 'Подразделение',
		dolz.znachenie as 'Должность',
		nach.fio as 'Фамилия ответственного'
--		IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka,
--		IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio,
		--obj.id as 'obj_id',
--		d.id as 'defect_id',
--		ms.id as 'msID',
--		rs.id as 'rsID',
--		nf.fileID as 'fileID'
		--od.lineID
		--obj.name		
from opres obj
	left join opresDeployed od ON od.directionID = obj.id
	left join linesobj l on l.id = od.lineID
	LEFT JOIN heatPipeSections hps ON hps.lineID = l.id
	LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID
	LEFT JOIN nodes n1 ON n1.id = srt.nodeID1
	LEFT JOIN nodes n2 ON n2.id = srt.nodeID2
	left join externalCodes ec1 ON ec1.id = n1.externalCodeID
	left join externalCodes ec2 ON ec2.id = n2.externalCodeID	
	left join defect d ON l.shape.STDistance(d.shape) < 0.1 and d.opresID = obj.id 
	LEFT JOIN defectTypes deft ON deft.id = d.remontTypeID
	LEFT JOIN stateDefect sd ON sd.id = d.stateID
	LEFT JOIN remontCat rc ON rc.id = d.remontCatID 
	left join nodes nf on nf.id = l.nodeID1
	left join vid_ispytani vid_is on vid_is.id = obj.vid_ispytaniID
	LEFT JOIN nachalniki_uchastkov nach ON nach.id = d.responsibleID
	LEFT JOIN dolzhnosti dolz ON dolz.id=nach.dolzhnost
	LEFT JOIN subdivisions subd ON subd.id = d.subdivisionID
	left join uchastok_ms ms ON ms.id = hps.magistralSite
	left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
	left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
	--left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
	left join uchastok_rs rs ON rs.id = hps.distSite
	left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
	left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
	where nf.fileID in ($fileID$) 
	AND ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) )
	and d.remontTypeID = 2
	and obj.id in ($objID$)
