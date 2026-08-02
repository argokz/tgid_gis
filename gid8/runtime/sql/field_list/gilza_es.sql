SELECT 
   gz.id, 
   CONCAT ( ISNULL(gz.nomer_gilzy_es,  '-'), ' ', 
	ISNULL(gz.tip__marka__harakteristika_, '-'), ' ',
	ISNULL(FORMAT (gz.data_ustanovki, 'dd-MM-yyyy'), '-')) as name
FROM gilza_es gz
join liniya_elektroperedach le on le.shape.STDistance(gz.shape) < 0.1
WHERE le.id = $fileID$
