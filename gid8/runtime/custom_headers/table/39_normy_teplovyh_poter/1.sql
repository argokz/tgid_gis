SELECT 
    t2,
    t1_1,
    t1_2,
    t1_3
FROM "39_normy_teplovyh_poter"
WHERE mode IS NULL 
  AND date = 1 
  AND proklad = 'Б' 
  AND tg = 5
ORDER BY d
LIMIT 1;