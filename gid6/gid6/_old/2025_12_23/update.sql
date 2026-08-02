UPDATE zdaniya_tu
SET shape = new_shape
FROM zdaniya_tu z
CROSS APPLY (
    SELECT Geometry::UnionAggregate(line) AS new_shape
    FROM (
        SELECT 
            CASE 
                WHEN geom.STGeometryType() = 'Polygon'
                    THEN geom.STBoundary()           -- все кольца (внешнее + дырки)
                WHEN geom.STGeometryType() IN ('LineString', 'MultiLineString')
                    THEN geom
                ELSE NULL
            END AS line
        FROM (
            SELECT z.shape.STGeometryN(n) AS geom
            FROM (
                SELECT TOP (z.shape.STNumGeometries()) 
                    ROW_NUMBER() OVER (ORDER BY (SELECT NULL)) AS n
                FROM master.dbo.spt_values   -- или sys.objects, или любая большая таблица
            ) nums
            WHERE n <= z.shape.STNumGeometries()
              AND z.shape.STGeometryType() = 'GeometryCollection'
        ) parts
        WHERE geom IS NOT NULL
          AND geom.STIsEmpty() = 0
          -- опционально: отсекаем "мусорные" полигоны с огромными координатами
          AND (geom.STGeometryType() <> 'Polygon' OR geom.STPointN(1).STX < 100000)
    ) t
    WHERE line IS NOT NULL
) ca
WHERE z.shape.MakeValid().STGeometryType() = 'GeometryCollection'
  AND ca.new_shape IS NOT NULL;

