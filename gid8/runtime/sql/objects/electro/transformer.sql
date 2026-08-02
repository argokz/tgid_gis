SELECT
	t.id,
	pe.naimenovanie_priemnika_es,
	tpe.znachenie as 'typ_priemnika',
	tp.name as 'transfPurpo',
	tt.name as 'transfType', 
	t.kol,
	t.pow,
	t.voltageHi,
	t.voltageLo,
	t.Ukz,
	t.Ixx,
	t.god	
FROM transf t 
LEFT JOIN priemnik_elektrosnabzheniya pe ON pe.id = t.objID 
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe ON tpe.id = pe.typID
LEFT JOIN transfPurpos tp ON tp.id = t.purposeID
LEFT JOIN transfTypes tt  ON tt.id = t.transfTypeID
ORDER BY naimenovanie_priemnika_es, god desc