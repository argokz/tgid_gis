SELECT t1_1, t1_2, t1_3, t1_4
FROM "39_normy_teplovyh_poter"
WHERE mode IS NULL AND date = 1 AND proklad = 'Н' AND tg = 5
LIMIT 1;