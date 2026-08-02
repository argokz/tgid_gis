SELECT
    heatSources.name,
    months.name,

--    tempView.tgP,
--    tempView.tgO,
--    tempView.tx,
--    tempView.dT,

    sum(if(tubingTypeID IN (1,2), 0, potP)) AS potNP,
    sum(if(tubingTypeID IN (1,2), 0, potO)) AS potNO,
    sum(if(tubingTypeID IN (1,2), potP+potO, 0)) AS potPodz,
    sum(potP+potO) AS potAll,
--    sum(tab1.V) AS V,
    sum(tab1.V*hlm.a/100*tempView.dT)/1E6 AS V1,
    sum(potP+potO+tab1.V*hlm.a/100*tempView.dT/1E6) AS Vall

FROM
(
    SELECT
        normmon.r,normmon.m,normmon.sezon,
        heatSourceID,typnet,tubingTypeID,y_norm,diameterExternal,diameterCondit,signNumWork,heatTestsCoeffP,heatTestsCoeffO,qp,qo,beta,
        sum(qp*lenP*beta*heatTestsCoeffP)/1E6 AS potP,
        sum(qo*lenO*beta*heatTestsCoeffO)/1E6 AS potO,

        sum(lenP) AS lenP,
        sum(lenO) AS lenO,
        sum((lenP+lenO)*PI()*POW(diameterExternal/1000,2)/4) AS V

    FROM normmon

    GROUP BY
      normmon.r,normmon.m,normmon.sezon,normmon.heatSourceID,typnet,tubingTypeID,beta,y_norm,diameterExternal,diameterCondit,signNumWork,heatTestsCoeffP,heatTestsCoeffO,qp,qo
) tab1

join heatLosesMain hlm ON hlm.id=1

JOIN tempView ON tempView.hID=heatSourceID AND tempView.r=tab1.r AND tempView.m <=12

LEFT JOIN heatSources ON heatSources.id=heatSourceID
LEFT JOIN months ON months.id=tab1.m

GROUP BY
    heatSourceID,
    tempView.tgP,
    tempView.tgO,
    tempView.tx,
    tempView.dT,
    tab1.m,
    tab1.r

ORDER BY
    heatSourceID,
    tab1.r
