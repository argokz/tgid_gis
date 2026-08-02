WITH RawParts AS (
    SELECT 
        n,
        @g.STGeometryN(n) AS geom
    FROM (
        SELECT TOP (@g.STNumGeometries()) 
            ROW_NUMBER() OVER (ORDER BY (SELECT NULL)) AS n
        FROM sys.objects  -- любой источник строк, главное много
    ) t
    WHERE @g.STGeometryType() = 'GeometryCollection'
),
Processed AS (
    SELECT 
        CASE 
            WHEN geom.STGeometryType() IN ('Polygon', 'MultiPolygon')
                THEN geom.STBoundary()          -- все кольца: внешние + дырки
            WHEN geom.STGeometryType() IN ('LineString', 'MultiLineString')
                THEN geom
            ELSE NULL
        END AS line_geom,
        
        -- для отладки
        geom.STGeometryType() AS original_type,
        geom.STNumGeometries() AS sub_count
    FROM RawParts
    WHERE geom IS NOT NULL
      AND geom.STIsEmpty() = 0
)



SELECT 
    -- основной вариант: MULTILINESTRING из всех границ
    Geometry::UnionAggregate(line_geom).STAsText() AS ResultMultiLineString,
    
    -- альтернативно: если хочешь просто собрать без union (иногда быстрее)
    -- Geometry::STGeomFromText(
    --     'MULTILINESTRING(' + STRING_AGG( CAST(line_geom.ToString() AS varchar(max)), ',') + ')', 
    --     line_geom.STSrid
    -- ) AS RawMultiLineString,
    
    COUNT(*) AS LineCount,
    STRING_AGG(original_type, ', ') AS Types
FROM Processed
WHERE line_geom IS NOT NULL;