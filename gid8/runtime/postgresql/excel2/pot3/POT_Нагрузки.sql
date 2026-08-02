SELECT 

sost,
obob,
kod,
uzel,
name_building,
round(otoplz,2),
round(otopln,1),
round(tp,1),
round(ventil,1),
round(kondiz,1),
round(Expr1010,1) AS e1,
round(Expr1011,1) As e2,
round(rez_q,2),
round(gvpr,2),
round(gvsm,2),
round(gvps,1),
round(gvpw,1),
round(q,2),
round(gvop,1),
round(gvoo,1),
round(v_otop,1),
round(v_vent,1),
name

FROM [POT_Нагрузки]
