SELECT 
    t1_1
    ,t1_2
    ,t1_3
    ,[d]
    ,qo_1
    ,qo_1+qp_1 as[qp_1]
    ,qo_1+qp_2 as[qp_2]
    ,qo_1+qp_3 as[qp_3]
FROM [sprav].[dbo].[39_normy_teplovyh_poter]
where date = 1 and proklad = 'Á' and tg = 5
order by d
