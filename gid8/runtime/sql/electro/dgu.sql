SELECT obj.id
      ,l.naimenovanie_lep
      ,tpr.znachenie
      ,pr.naimenovanie_priemnika_es
      ,t3.name
      ,description
      ,pow
FROM dgu obj
LEFT JOIN priemnik_elektrosnabzheniya pr ON pr.id=obj.objID
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpr ON tpr.id=pr.typID
LEFT JOIN dguPurpose t3 ON t3.id=obj.purposeID

LEFT JOIN liniya_elektroperedach l ON l.id=pr.naimenovanie_lep
