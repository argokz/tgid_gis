SELECT
period,
sum(length),
sum(length*t1)/sum(length),
sum(length*t2)/sum(length),
sum(length*t)/sum(length)

FROM TEMP_IST_OUT

GROUP BY 
  period

UNION ALL

SELECT
2,
sum(length),
sum(length*t1)/sum(length),
sum(length*t2)/sum(length),
sum(length*t)/sum(length)

FROM TEMP_IST_OUT
