SELECT
	ed.id,
	pe.naimenovanie_priemnika_es,
	tpe.znachenie as 'typ_priemnika',
	marka_edv,
	ep.name as 'edvPurpos',
	te.name as 'typeEdv', 
	ed.pow,
	ed.freq,
	ed.seria,
	ed.diap,
	ed.prec
FROM edv ed 
LEFT JOIN priemnik_elektrosnabzheniya pe ON pe.id = ed.objID 
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe ON tpe.id = pe.typID
LEFT JOIN edvPurpos ep ON ep.id = ed.purposeID
LEFT JOIN typeEdv te  ON te.id = ed.edvTypeID


