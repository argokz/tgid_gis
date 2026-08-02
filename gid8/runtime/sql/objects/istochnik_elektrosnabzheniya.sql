SELECT
	ie.id,
	ie.naimenovanie_istochnika_es as 'Наименование источника ЭС',
	te.znachenie as 'Тип источника',
	vld.naimenovanie as 'Владелец источника ЭС',
	ie.primechanie as 'Примечание'
FROM istochnik_elektrosnabzheniya ie
LEFT JOIN vladeltsy_es vld on vld.id = ie.vladeltsy_es_ID
LEFT JOIN tipy_istochnikov_elektricheskih_setey te on te.id = ie.typID