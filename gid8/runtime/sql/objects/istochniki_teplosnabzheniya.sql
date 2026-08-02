SELECT 
distinct
 obj.id,
 IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Начальный узел',
 IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Конечный узел', 
 obj.naimenovanie_istochnika AS 'Наименование источника',
 obj.tip_istochnika AS 'Тип источника',
 obj.shema AS 'Схема'
 FROM istochniki_teplosnabzheniya obj
 LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < 0.3
 LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
 LEFT JOIN pipeSections pipe ON pipe.id=hps.pipeSectionID
 LEFT JOIN sortLinesForUchastok srt ON srt.pipeSectionID=pipe.id
 LEFT JOIN nodes n1 ON n1.id=pipe.nodeID1
 LEFT JOIN nodes n2 ON n2.id=pipe.nodeID2
 where 1=1 $AND_N1FILEID$