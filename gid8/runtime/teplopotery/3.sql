SELECT 
id,
dy,
qp_1,
qo_1,
qp_2,
qo_2,
qp_3,
qo_3,
qp_1gt5000,
qo_1gt5000,
qp_2gt5000,
qo_2gt5000,
qp_3gt5000,
qo_3gt5000
FROM [sprav].[dbo].[39_normy_teplovyh_poter]
where date = 2 and proklad = 'Ê'
order by dy


