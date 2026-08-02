
select 
	distinct
	 obj.id 
	,distance.beginNode AS 'Начальный узел'
	,distance.endNode AS 'Конечный узел'
	--,distance.fileID
	,rt.name as 'remontType'
	,rc.name as 'remontCat'
    ,data_nachala_remonta
    ,data_zaversheniya_remonta     
    ,otchet_po_defektu
	,remontTypeID
	,workListTube =  case remontTypeID
	  when 1 then   STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontInvestTube rt2 ON r2.id=rt2.objID
					LEFT JOIN remontInvestTubeTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '') 	  
	  when 2 then   STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontCapitalTube rt2 ON r2.id=rt2.objID
					LEFT JOIN remontCapitalTubeTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	  when 3 then STUFF(
					( SELECT CHAR(10) + rtt2.name AS n FROM remont r2 
					LEFT JOIN remontTube rt2 ON r2.id=rt2.objID
					LEFT JOIN remontTubeTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	  end 
	  ,workListChannel =  case remontTypeID
	  when 1 then STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontInvestChannel rt2 ON r2.id=rt2.objID
					LEFT JOIN remontInvestChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '') 	 
	  when 2 then   STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontCapitalChannel rt2 ON r2.id=rt2.objID
					LEFT JOIN remontCapitalChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	 when 3 then STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontChannel rt2 ON r2.id=rt2.objID
					LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')	
	  end
	  ,workListKamera = case remontTypeID	
	  when 1 then   STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontInvestKamera rt2 ON r2.id=rt2.objID
					LEFT JOIN remontInvestChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '') 
	  when 2 then   STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontCapitalKamera rt2 ON r2.id=rt2.objID
					LEFT JOIN remontCapitalChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	  when 3 then STUFF(
					( SELECT CHAR(10)+rtt2.name AS n FROM remont r2 
					LEFT JOIN remontKamera rt2 ON r2.id=rt2.objID
					LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
					WHERE r2.id=obj.id 
					FOR XML PATH('')
					, TYPE
					).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
	  end	  
	  ,case remontTypeID 
		  when 3 then len_tube_cur
		  when 2 then len_tube_cap
		  when 1 then len_tube_inv
		  end AS len_tube
	  ,case remontTypeID 
		  when 3 then len_izol_cur
		  when 2 then len_izol_cap
		  when 1 then len_izol_inv
		  end AS len_izol
	  ,case remontTypeID 
		  when 3 then len_channel_cur
		  when 2 then len_channel_cap
		  when 1 then len_channel_inv
		  end AS len_channel
	  ,subd.name as subdivision
	  ,d.znachenie as dolzhnost
      ,nach.fio as fio_otv
	  ,IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka
	  ,IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio
	  ,obj.primechanie
	  ,distance.uchastok_ms_id as 'ms'
	  ,distance.uchastok_rs_id as 'rs'
	  --,distance.lineID 
	  --,distance.length
	  --,distance.min_len 
from remont obj
LEFT JOIN (SELECT
			  obj.id,
			  line.beginNode,
			  line.endNode,
			  line.fileID,
			  line.id as 'lineID',
			  line.uchastok_ms_id,
			  line.uchastok_rs_id,
			  line.shape.STDistance(obj.shape) as length,
			  MIN(line.shape.STDistance(obj.shape)) OVER(PARTITION BY obj.id ) AS "min_len"
		FROM remont obj
		LEFT JOIN ( select
						l.id, 
						IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
						IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
						l.shape, 
						srt.uchastok_ms_id, 
						srt.uchastok_rs_id,
						n1.fileID
					from linesobj l
					LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
					LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID	
					LEFT JOIN nodes n1 ON n1.id = srt.nodeID1 
					LEFT JOIN nodes n2 ON n2.id = srt.nodeID2
					LEFT JOIN externalCodes ec1 ON ec1.id = n1.externalCodeID
					LEFT JOIN externalCodes ec2 ON ec2.id = n2.externalCodeID		
					where n1.id is not null and n2.id is not null and n1.removed = 0 and n2.removed = 0 and l.removed = 0 and n1.fileID = n2.fileID 
					and n1.fileID in ($fileID$) 
					AND ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null))) line ON line.shape.STDistance(obj.shape) < 0.2
			) distance ON distance.id = obj.id 
	
LEFT JOIN stateDefect sd ON sd.id = obj.stateID
LEFT JOIN remontCat rc ON rc.id = obj.remontCatID 
LEFT JOIN remontTypes rt ON rt.id=remontTypeID 
LEFT JOIN nachalniki_uchastkov nach ON nach.id=obj.responsibleID
LEFT JOIN dolzhnosti d ON d.id=nach.dolzhnost
LEFT JOIN subdivisions subd ON subd.id=subdivisionID
LEFT JOIN uchastok_ms ms ON ms.id = distance.uchastok_ms_id
LEFT JOIN uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
LEFT JOIN nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
--left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
LEFT JOIN uchastok_rs rs ON rs.id = distance.uchastok_rs_id
LEFT JOIN uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
LEFT JOIN nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
--left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
    
$j$

where distance.lineID is not null 
and distance.length = distance.min_len
$and_condition$

$order$
