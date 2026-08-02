SELECT transf.id
      ,l.naimenovanie_lep
      ,tpr.znachenie
      ,pr.naimenovanie_priemnika_es
      ,t4.name
      ,t5.name
      ,description
      ,pow
      ,voltageHi
      ,voltageLo
      ,Ukz
      ,Ixx
      ,kol
      ,god

  
  FROM transf
  LEFT JOIN priemnik_elektrosnabzheniya pr ON pr.id=transf.objID
  LEFT JOIN tipy_priemnikov_elektricheskih_setey tpr ON tpr.id=pr.typID
  LEFT JOIN liniya_elektroperedach l ON l.id=pr.naimenovanie_lep

  LEFT JOIN transfPurpos t4 ON t4.id=transf.purposeID

  LEFT JOIN transfTypes t5 ON t5.id=transf.transfTypeID


  ORDER BY transf.objID


