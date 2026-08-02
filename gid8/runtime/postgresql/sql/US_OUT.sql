SELECT 
n.id,
us1.id AS id2,
ec.name AS 'Код РС',
n.externalNodeName AS 'Наименование', 
n.geoMarkTopTube AS 'Геодезическая отметка, м',

IIF(NOT us1.pih IS NULL AND NOT us2.pih IS NULL, us1.pih-us2.pih, 0) AS 'Располагаемый напор, м',
us1.pih AS 'Давление в подающем трубопроводе, м', 
us2.pih AS 'Давление в обратном трубопроводе, м',
us1.t AS 'Температура в подающем трубопроводе, °C',
us2.t AS 'Температура в обратном трубопроводе, °C'

FROM nodes n
left join US_OUT us1 ON us1.nodeID=n.id AND us1.externalSign=1 AND us1.calculationID=$calculationID$
left join US_OUT us2 ON us2.nodeID=n.id AND us2.externalSign=2 AND us2.calculationID=$calculationID$
join externalCodes ec on ec.id = n.externalCodeID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0 
