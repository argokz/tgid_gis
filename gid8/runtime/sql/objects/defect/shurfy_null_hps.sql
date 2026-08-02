SELECT 
	d.id

FROM shurfy d 
LEFT JOIN linesobj l ON l.shape.STDistance(d.shape) < 0.1 AND l.removed=0 
WHERE l.id IS NULL