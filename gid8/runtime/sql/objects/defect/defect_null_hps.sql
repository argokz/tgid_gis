SELECT 
	d.id,
	FORMAT(d.data_osmotra,'dd.MM.yyyy' ) as 'Дата обнаружения нарушения',
	deft.name as 'Режим',
	vid_rabot.name as 'Вид работ',
	stateDefect.name as 'Состояние',
	d.defectDescription as 'Описание повреждения',
	d.otchet_po_defektu as 'Примечание',
	op.name as 'Наименование контура опрессовки',
    CONCAT(st.name, '', d.nomer_doma) as 'Адрес',
	vn.name as 'Вид нарушения',
    rc.name as 'Категория нарушения',
    tpov.name as 'Характер повреждения'
FROM defect d 
LEFT JOIN linesobj l ON l.shape.STDistance(d.shape) < 0.1 AND l.removed=0 
left join ulitsy st ON st.id = d.ulicaID
LEFT JOIN defectTypes deft ON deft.id = d.remontTypeID
left join stateDefect on stateDefect.id = d.stateID
left join opres op ON op.id = d.opresID
left join vid_narusheniya vn on vn.id = d.vid_narusheniyaID
left join vid_rabot on vid_rabot.id = d.vid_rabotID
left join remontCat rc ON rc.id = d.remontCatID
left join tipPovrezhdenia tpov ON  tpov.id = d.tipPovrezhdeniaID
WHERE l.id IS NULL