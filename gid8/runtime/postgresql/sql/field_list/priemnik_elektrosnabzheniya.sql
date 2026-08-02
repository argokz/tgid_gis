SELECT 
   obj.id,

    CONCAT ( ISNULL(obj.naimenovanie_priemnika_es, '-'), ' (', 
	ISNULL(t.znachenie, '-'), ' )') AS name



FROM priemnik_elektrosnabzheniya obj 
JOIN liniya_elektroperedach l ON l.shape.STDistance(obj.shape) < 0.1	
LEFT JOIN tipy_istochnikov_elektricheskih_setey t ON t.id=obj.typID
WHERE l.id=$fileID$
