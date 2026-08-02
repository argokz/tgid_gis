SELECT top 2147483647 name, val 
FROM (
  SELECT 1 AS id1, id, 'Координаты точки начала рабочей зоны, расход, т/ч' AS name, q_min AS val FROM standardPumps
  UNION SELECT 2, id, 'Координаты точки начала рабочей зоны, напор, м.вод.ст.', h_min FROM standardPumps
  UNION SELECT 3, id, 'Координаты точки конца рабочей зоны, расход, т/ч', q_max FROM standardPumps
  UNION SELECT 4, id, 'Координаты точки конца рабочей зоны, напор, м.вод.ст.', h_max FROM standardPumps
  UNION SELECT 5, id, 'Расход воды через насос номинальный, т/ч', q_nomin FROM standardPumps
  UNION SELECT 6, id, 'Напор воды через насос номинальный, м.вод.ст.', h_nomin FROM standardPumps
  UNION SELECT 7, id, 'КПД насоса номинальный', k_nomin FROM standardPumps
  UNION SELECT 8, id, 'Диаметр рабочего колеса номинальный', d_nomin FROM standardPumps
  UNION SELECT 9, id, 'Частота вращения рабочего колеса номинальная', rate_nomin FROM standardPumps
  UNION SELECT 10,id, 'Количество рабочих колес', kol_d FROM standardPumps
  UNION SELECT 11,id, 'Температура рабочей среды максимальная',t_max  FROM standardPumps
  UNION SELECT 12,id, 'Давление воды на всасе насоса, м.вод.ст.',p_home  FROM standardPumps
  UNION SELECT 13,id, 'Высота всасывания вакууметрическая максимальная',h_home  FROM standardPumps
  --UNION SELECT 14,id, 'Завод-изготовитель',producer  FROM standardPumps
  UNION SELECT 14,id, 'Давление рабочее на входе максимальное, м.вод.ст.',p_home_max  FROM standardPumps
  UNION SELECT 15,id, 'Масса', massa FROM standardPumps
) st
JOIN pumps ON pumps.standardPumpID=st.id OR pumps.standardPumpID IS NULL AND st.id=1
WHERE pumps.lineID=$id$
ORDER BY id1
