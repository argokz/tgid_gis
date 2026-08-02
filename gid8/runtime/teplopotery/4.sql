
SELECT 
dy,
qp_1,
qo_1,
qp_2,
qo_2,
qp_1gt5000,
qo_1gt5000,
qp_2gt5000,
qo_2gt5000
FROM [sprav].[dbo].[39_normy_teplovyh_poter]
where date = 2 and proklad = 'Á'
order by dy