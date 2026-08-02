SELECT 1 as "n", 'Напор, м' AS "№ точки", h1 as "1", h2 as "2", h3 as "3", h4 as "4", h5 as "5", h6 as "6", h7 as "7", h8 as "8", h9 as "9", h10 as "10" FROM standardPumps WHERE id = $id$ UNION
SELECT 2 as "n", 'Мощность, Квт' AS "№ точки", q1, q2, q3, q4, q5, q6, q7, q8, q9, q10 FROM standardPumps WHERE id = $id$ UNION
SELECT 3 as "n", 'КПД, %' AS "№ точки", k1, k2, k3, k4, k5, k6, k7, k8, k9, k10 FROM standardPumps WHERE id = $id$ UNION
SELECT 4 as "n", 'Расход, т/ч' AS "№ точки", n1, n2, n3, n4, n5, n6, n7, n8, n9, n10 FROM standardPumps WHERE id = $id$ ORDER BY "n";