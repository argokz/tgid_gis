SELECT obj.id
      ,l.naimenovanie_lep
      ,tpr.znachenie
      ,pr.naimenovanie_priemnika_es
      ,t3.name
      ,t2.name
      ,description
      ,pow
      ,freq
      ,seria
      ,diap
      ,prec
  FROM edv obj
  LEFT JOIN priemnik_elektrosnabzheniya pr ON pr.id=obj.objID
  LEFT JOIN tipy_priemnikov_elektricheskih_setey tpr ON tpr.id=pr.typID
  LEFT JOIN typeEdv t2 ON t2.id=obj.edvTypeID
  LEFT JOIN edvPurpos t3 ON t3.id=obj.purposeID

  LEFT JOIN liniya_elektroperedach l ON l.id=pr.naimenovanie_lep

  ORDER BY obj.objID
