SELECT 
pc.id, 
pc.name AS "Наименование",
ot.name AS "Объект РС",
-- ue.nomer_uchastka AS "Участок эксплуатации",
ec.name AS "Принадлежность магистрали",
hs.name AS "Источник тепла",
pc.responsiblePerson AS "Ответственный"


FROM externalCodes pc
LEFT JOIN heatSources hs ON hs.id = pc.heatSourceID
LEFT JOIN objectTypes ot ON ot.id = pc.objectID

-- LEFT JOIN uchastki_ekspluatatsii ue ON ue.id = pc.exploitSite
LEFT JOIN externalCodes ec ON ec.id = pc.belongMagistral

WHERE pc.fileID=$fileID$
