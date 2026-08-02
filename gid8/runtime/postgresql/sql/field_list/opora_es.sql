SELECT 
	oe.id,
	CONCAT ( ISNULL(oe.nomer_opory_es, '-'), ' ', 
	ISNULL(oe.tip__marka__harakteristika_, '-'), ' ',
ISNULL(FORMAT (oe.data_ustanovki, 'dd-MM-yyyy'), '-')) as name
From opora_es oe 
join liniya_elektroperedach le on le.shape.STDistance(oe.shape) < 0.1
where le.id = $fileID$