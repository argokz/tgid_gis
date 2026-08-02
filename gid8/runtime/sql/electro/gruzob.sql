SELECT obj.id
      ,l.naimenovanie_lep
      ,tpr.znachenie
      ,pr.naimenovanie_priemnika_es
      ,t2.name
      ,description
      ,kol
  FROM gruzob obj
  LEFT JOIN priemnik_elektrosnabzheniya pr ON pr.id=obj.objID
  LEFT JOIN tipy_priemnikov_elektricheskih_setey tpr ON tpr.id=pr.typID
  LEFT JOIN gruPurpose t2 ON t2.id=obj.typeID

  LEFT JOIN liniya_elektroperedach l ON l.id=pr.naimenovanie_lep
