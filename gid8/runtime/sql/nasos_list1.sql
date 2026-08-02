SELECT top 2147483647 1 AS 'n', 'Напор, м' AS '№ точки', h1 AS '1',h2 AS '2',h3 AS '3',h4 AS '4',h5 AS '5',h6 AS '6',h7 AS '7',h8 AS '8',h9 AS '9',h10 AS '10' FROM standardPumps WHERE id=$id$
UNION SELECT top 2147483647 2, 'Мощность, Квт', q1,q2,q3,q4,q5,q6,q7,q8,q9,q10 FROM standardPumps WHERE id=$id$
UNION SELECT top 2147483647 3, 'КПД, %', k1,k2,k3,k4,k5,k6,k7,k8,k9,k10 FROM standardPumps WHERE id=$id$
UNION SELECT top 2147483647 4, 'Расход, т/ч', n1,n2,n3,n4,n5,n6,n7,n8,n9,n10 FROM standardPumps WHERE id=$id$

