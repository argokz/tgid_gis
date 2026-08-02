SELECT transf.id
      ,pr.naimenovanie_priemnika_es
      ,god
      ,description
      ,Ukz
      ,Ixx
      ,kol
      ,pow
      ,voltageHi
      ,voltageLo
  FROM transf
  LEFT JOIN priemnik_elektrosnabzheniya pr ON pr.id=transf.objID
  ORDER BY transf.objID