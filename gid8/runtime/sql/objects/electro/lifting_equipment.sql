SELECT
	gz.id,
	pe.naimenovanie_priemnika_es,
	tpe.znachenie as 'typ_priemnika',
	gp.name as 'gzTypePurpose',
	gz.description,
	gz.primechanie 
FROM gruzob gz
LEFT JOIN priemnik_elektrosnabzheniya pe ON pe.id = gz.objID 
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe ON tpe.id = pe.typID
LEFT JOIN gruPurpose gp ON gp.id = gz.typeID
ORDER BY naimenovanie_priemnika_es