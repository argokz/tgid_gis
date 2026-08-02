SELECT
	dg.id,
	pe.naimenovanie_priemnika_es,
	tpe.znachenie as 'typ_priemnika',
	dp.name as 'dgPurpose',
	dg.pow,
	dg.description,
	dg.primechanie 
FROM dgu dg
LEFT JOIN priemnik_elektrosnabzheniya pe ON pe.id = dg.objID 
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe ON tpe.id = pe.typID
LEFT JOIN dguPurpose dp ON dp.id = dg.purposeID
ORDER BY naimenovanie_priemnika_es