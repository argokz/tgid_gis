SELECT 
   obj.id,

    CONCAT ( ISNULL(obj.naimenovanie_istochnika_es, '-'), ' (', 
	ISNULL(t.znachenie, '-'), ' )') AS name

FROM istochnik_elektrosnabzheniya obj 
JOIN liniya_elektroperedach l ON l.shape.STDistance(obj.shape) < 0.1	
LEFT JOIN tipy_priemnikov_elektricheskih_setey t ON t.id=obj.typID
WHERE l.id=$fileID$
