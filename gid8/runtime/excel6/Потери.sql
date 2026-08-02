SELECT
  heatSources.sourceName AS ist,

--  typnet,

  dbo.getTypnet(typnet) as typnet1,

  IIF(y_norm=1, 'Норма 1',

  CONCAT('Норма ', y_norm, ', ', IIF(signNumWork <> 0, ' больше', 'меньше'), ' 5000 часов работы')
  ) AS a5000,

--  '',



--    CONCAT('Норма ', y_norm) AS y_norm,

--    IIF(signNumWork, 'Больше 5000 часов', 'Меньше 5000 часов'),
    diameterCondit,

    ROUND(lenNP,2)+0,
    ROUND(lenNO,2)+0,
    ROUND(qNP,2)+0,
    ROUND(qNO,2)+0,
    ROUND(potNP,1)+0,
    ROUND(potNO,1)+0,

    ROUND(lenKP,2)+0,
    ROUND(lenKO,2)+0,
    ROUND(qK,2)+0,

    ROUND(lenBP,2)+0,
    ROUND(lenBO,2)+0,
    ROUND(qB,2)+0,

    ROUND(potKP+potBP+potKO+potBO,1)+0 AS potP

FROM (

SELECT
    heatSourceID,
    typnet,
    y_norm,
    signNumWork,
--    IIF(y_norm=1, diameterExternal,
    diameterCondit,

--    '|',
-- ----------------------- Надземная -------------------------------------------------

    sum(IIF(tubingTypeID IN (3,4,5), lenP, 0)) AS lenNP,
    sum(IIF(tubingTypeID IN (3,4,5), lenO, 0)) AS lenNO,
    sum(IIF(tubingTypeID IN (3,4,5), qp, 0)) AS qNP,
    sum(IIF(tubingTypeID IN (3,4,5), qo, 0)) AS qNO,
    sum(IIF(tubingTypeID IN (3,4,5), potP, 0)) AS potNP,
    sum(IIF(tubingTypeID IN (3,4,5), potO, 0)) AS potNO,

-- ------------------------------ Канальная -----------------------------------------------------
--    '|',

    sum(IIF(tubingTypeID = 1, lenP, 0)) AS lenKP,
    sum(IIF(tubingTypeID = 1, lenO, 0)) AS lenKO,
    sum(IIF(tubingTypeID = 1, qp+qo, 0)) AS qK,
    sum(IIF(tubingTypeID = 1, potP, 0)) AS potKP,
    sum(IIF(tubingTypeID = 1, potO, 0)) AS potKO, 

-- ------------------------------- Бесканальня ------------------------------------
--    '|',

    sum(IIF(tubingTypeID = 2, lenP, 0)) AS lenBP,
    sum(IIF(tubingTypeID = 2, lenO, 0)) AS lenBO,
    sum(IIF(tubingTypeID = 2, qp+qo, 0)) AS qB,
    sum(IIF(tubingTypeID = 2, potP, 0)) AS potBP,
    sum(IIF(tubingTypeID = 2, potO, 0)) AS potBO



FROM
(

SELECT
    heatSourceID,
    typnet,
    tubingTypeID,
    y_norm,
    diameterExternal,
    diameterCondit,
    signNumWork,
    heatTestsCoeffP,
    heatTestsCoeffO,
    qp,
    qo,
    beta,
    sum(qp*lenP*beta*heatTestsCoeffP) AS potP,
    sum(qo*lenO*beta*heatTestsCoeffO) AS potO,

    sum(lenP) AS lenP,
    sum(lenO) AS lenO

FROM normmon WHERE normmon.m=13

GROUP BY
    heatSourceID,
    typnet,
    tubingTypeID,
    beta,
    y_norm,
    diameterExternal,
    diameterCondit,
    signNumWork,
    heatTestsCoeffP,
    heatTestsCoeffO,
    qp,
    qo

) _TTT

GROUP BY
    heatSourceID,
    typnet,
    signNumWork,
    y_norm,
--    diameterExternal,
    diameterCondit

) _TTT1

LEFT JOIN heatSources ON heatSources.id=heatSourceID

ORDER BY
    heatSourceID,
    typnet,
    y_norm,
    signNumWork DESC,
    diameterCondit
