SELECT 
	kk.id, 
	CONCAT ( ISNULL(kk.nomer_kanala_es, '-'), ' ', 
	ISNULL(kk.tip__marka__harakteristika_, '-'), ' ',
	ISNULL(FORMAT (kk.data_ustanovki, 'dd-MM-yyyy'), '-')) as name
FROM kabelnyy_kanal_es kk
join liniya_elektroperedach le on le.shape.STDistance(kk.shape) < 0.1
where le.id = $fileID$