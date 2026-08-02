SELECT 
	mt.id,
	CONCAT ( ISNULL(mt.nomer_mufty_es,  '-'), ' ', 
	ISNULL(mt.tip__marka__harakteristika_, '-'), ' ',
	ISNULL(FORMAT (mt.data_ustanovki, 'dd-MM-yyyy'), '-')) as name
FROM mufta mt
join liniya_elektroperedach le on le.shape.STDistance(mt.shape) < 0.1
WHERE le.id = $fileID$
