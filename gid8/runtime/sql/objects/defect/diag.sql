
SELECT 
    t.id,    
    l_id,
    t.beginNode AS 'Начальный узел',
	t.endNode AS 'Конечный узел',
	t.diam_usl AS 'Диаметр условный, мм',
    t.externalSignLineStr AS 'Признак трубопровода',
--	t.priznak_truboprovoda,
	t.data_prov,
	t.appearance AS 'Внешний вид',
	t.equipment AS 'Оборудование',
	t.construction AS 'Состояние строительных конструкций',
	t.otchet_osv,	

    t.subdivision AS 'Подразделение производившее работы',
	t.dolzhnost AS 'Должность ответственного',
	t.fio_otv AS 'ФИО ответственного',

--	t.naimenovanie_uchastka AS '',
	t.fio AS 'Начальник участка'

--	t.naimenovanie_uchastka,
--	t.fio,

--	t.primechanie,
--	t.orderID,
--	t.id as 'id_obj',
--	t.length,
--	t.min_len,
--	t.ms,
--	t.rs,
--'' AS 'qq'
	FROM (
	  SELECT 
	  IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
	  IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
      l.id AS l_id,
	  l.shape.STDistance(obj.shape) as length,
      MIN(l.shape.STDistance(obj.shape)) OVER(PARTITION BY obj.id ) AS "min_len",
	  srt.orderID,
	  obj.id,
	  obj.diam_usl,
	  obj.priznak_truboprovoda,
	  obj.data_prov,
	  vid.name as appearance,
	  oborud.name as equipment,
	  konstr.name as construction,
	  obj.otchet_osv,
	  obj.primechanie,
	  subd.name as subdivision
	  ,d.znachenie as dolzhnost
      ,nach.fio as fio_otv
	  ,IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka
	  ,IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio
	  ,srt.uchastok_ms_id as 'ms'
	  ,srt.uchastok_rs_id as 'rs'
	  ,es.name as externalSignLineStr 
FROM diag obj
LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < 0.1
LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID	
LEFT JOIN nodes n1 ON n1.id = srt.nodeID1
LEFT JOIN nodes n2 ON n2.id = srt.nodeID2
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID	
LEFT JOIN externalSignLine es ON es.id = obj.priznak_truboprovoda
LEFT JOIN vneshny_vid vid ON vid.id = obj.vneshny_vidID
LEFT JOIN sost_oborud oborud ON oborud.id = obj.sost_oborudID
LEFT JOIN sost_konstr konstr ON konstr.id = obj.sost_konstrID

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
where t.length = t.min_len	


$order$
