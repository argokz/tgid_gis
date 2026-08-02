insert into [39_normy_teplovyh_poter] (d, dy, date, proklad, tg, tn, t2, t1_1, t1_2, t1_3, t1_4, qp_1, qo_1, qp_2, qo_2, qp_3, qo_3, qp_4, qo_4, qp_1gt5000, qo_1gt5000, qp_2gt5000, qo_2gt5000, qp_3gt5000, qo_3gt5000, qp_4gt5000, qo_4gt5000, mode)
select d, dy, date, proklad, tg, tn, t2, t1_1, t1_2, t1_3, t1_4, qp_1, qo_1, qp_2, qo_2, qp_3, qo_3, qp_4, qo_4, qp_1gt5000, qo_1gt5000, qp_2gt5000, qo_2gt5000, qp_3gt5000, qo_3gt5000, qp_4gt5000, qo_4gt5000, 1 as 'mode' from [39_normy_teplovyh_poter]

