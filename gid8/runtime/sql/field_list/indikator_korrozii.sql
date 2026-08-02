SELECT		
		obj.id,
		CONCAT('Индикатор коррозии № ', obj.id,
		' ( ',ISNULL(el.name,'признак трубопровада не указан'),' ), ',
		'состояние : ',ISNULL( sost.name, 'состояние не указано')) name
--        ,		srt.pipeSectionID
FROM indikator_korrozii obj		
LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape.STPointN(1)) < 0.1
LEFT JOIN externalSignLine el ON el.id = obj.truboprovod
LEFT JOIN stateIndicator sost ON sost.id = obj.sostoyanie
LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID	
LEFT JOIN nodes n1 ON n1.id = srt.nodeID1
LEFT JOIN nodes n2 ON n2.id = srt.nodeID2
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID	
WHERE NOT l.shape.STDistance(obj.shape.STPointN(1)) IS NULL and n1.fileID in ( $FragmentIds$ )
and srt.pipeSectionID = $fileID$
and l.externalSignLineID is not NULL