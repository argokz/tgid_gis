SELECT 
    tn.id as 'Номер', 
    IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Начальный узел',
    IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Конечный узел', 
    el.name as 'Признак трубопровода',
    prinadlezhnost as 'Принадлежность',
    --nn.nodeName   as 'Наименование узла/павильона',
    --tn.nodeID,
    --tn.lineID,
    loc.name as 'Месторасположение',
    art.name as 'Тип арматуры',
    dt.name as 'Исполнение',
    mt.name as 'Материал',
    ct.name as 'Конструкция',
    diametr as 'Диаметр',   
    rabochee_davlenie as 'Рабочее давление',
    god_vvoda as 'Год ввода',
--  nomer as 'Номер',
    shema as 'Схема',
    tn.primechanie as 'Примечание'
    ,l.id AS l_id
FROM drenazhnyy_kran tn
LEFT JOIN linesobj l ON l.id =  tn.lineID
--LEFT JOIN linesobj l ON l.shape.STDistance(tn.shape.STPointN(1)) < 0.1
LEFT JOIN heatPipeSections hps ON hps.lineID = l.id
LEFT JOIN nodes n1 ON n1.id = l.nodeID1
LEFT JOIN nodes n2 ON n2.id = l.nodeID2
--LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID

--LEFT JOIN nodes sn1 ON sn1.id = srt.nodeID1
--LEFT JOIN nodes sn2 ON sn2.id = srt.nodeID2
--LEFT JOIN nodes nn ON nn.shape.STDistance(tn.shape.STPointN(1)) < 3
LEFT JOIN externalSignLine el ON el.id = tn.priznak_truboprovoda
LEFT JOIN Locations loc ON loc.id = tn.mestoraspolozhenie
LEFT JOIN designTypes dt ON dt.id = tn.designTypesID
LEFT JOIN constructionTypes ct ON ct.id = tn.constructionTypesID
LEFT JOIN armatureTypes art ON art.id = tn.armatureTypesID
LEFT JOIN materialTypes mt ON mt.id = tn.materialTypesID
