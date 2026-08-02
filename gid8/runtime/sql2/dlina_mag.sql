SELECT 
-- magistral, 
m.naimenovanie_magistrali AS Магистраль,
diameterCondit AS Диаметр,
sum(lenP) AS Подающий,
sum(lenO) AS Обратный

FROM 
(
SELECT
hps.magistral,
hps.diameterCondit,
--m.naimenovanie_magistrali, 
IIF(l.externalSignLineID IN (1,2,4),  hps.pipeSectLength, 0) AS lenP,
IIF(l.externalSignLineID IN (1,3,5),  hps.pipeSectLength, 0) AS lenO

FROM heatPipeSections hps
JOIN linesobj l ON hps.lineID=l.id
) _TT

LEFT JOIN magistrali m ON m.id=_TT.magistral

GROUP BY 
  magistral,
    naimenovanie_magistrali,
  diameterCondit


ORDER BY 
    naimenovanie_magistrali,
  diameterCondit

