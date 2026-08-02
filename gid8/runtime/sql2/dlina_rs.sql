SELECT 
-- magistral, 
ue.nomer_uchastka AS 'Номер участка',
nach.fio AS 'Начальник участка',
naimenovanie_uchastka_rs AS 'Участок РС',
diameterCondit AS Диаметр,
sum(lenP) AS Подающий,
sum(lenO) AS Обратный

FROM 
(
SELECT
hps.distSite,
hps.diameterCondit,
--m.naimenovanie_magistrali, 
IIF(l.externalSignLineID IN (1,2,4),  hps.pipeSectLength, 0) AS lenP,
IIF(l.externalSignLineID IN (1,3,5),  hps.pipeSectLength, 0) AS lenO

FROM heatPipeSections hps
JOIN linesobj l ON hps.lineID=l.id
) _TT

LEFT JOIN uchastok_rs rs ON rs.id=_TT.distSite
LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=rs.nomer_uchastka
LEFT JOIN nachalniki_uchastkov nach ON ue.nachalnik_uchastka=nach.id

GROUP BY 
nach.fio,
  distSite,
  ue.nomer_uchastka,
  naimenovanie_uchastka_rs,
  diameterCondit


ORDER BY 
nach.fio,
  ue.nomer_uchastka,
  distSite,
  naimenovanie_uchastka_rs,
  diameterCondit

