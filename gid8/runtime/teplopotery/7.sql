SELECT 
id,
t1_1,
t1_2,
t1_3,
dy,
qp_1,
qp_2,
qp_3
FROM [sprav].[dbo].[39_normy_teplovyh_poter]
where date = 3 and proklad = 'Í'
order by dy