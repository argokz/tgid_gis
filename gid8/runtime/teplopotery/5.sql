SELECT 
    t1_1,
    t1_2,
    t1_3
    ,[dy]
    ,[qp_1]
    ,[qp_2]
    ,[qp_3]
    ,[qp_1gt5000]
    ,[qp_2gt5000]
    ,[qp_3gt5000]
FROM [sprav].[dbo].[39_normy_teplovyh_poter]
where date = 2 and proklad = 'Í' 
order by dy