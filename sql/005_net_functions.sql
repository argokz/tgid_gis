-- Функции, необходимые конвертеру. Применять до 010_net_schema.sql.

CREATE SCHEMA IF NOT EXISTS net;

-- Разбор текстового поля linesobj.coords в массив точек.
--
-- Формат: числа, разделённые пробелами и/или запятыми вперемешку, парами x y.
-- Внутренние единицы ТГИД — сантиметры, ось Y направлена вниз, поэтому
-- x -> x/scale, y -> -y/scale. Повторяющиеся подряд точки отбрасываются —
-- так же поступает CCoordList::loadStr в C++ (cxema/coordlis.cpp:1140).
--
-- Важно: coords содержит ТОЛЬКО промежуточные вершины линии.
-- Концы всегда берутся из узлов nodeid1 / nodeid2.
CREATE OR REPLACE FUNCTION net.parse_coords(txt text, scale float8 DEFAULT 100.0)
RETURNS geometry[]
LANGUAGE plpgsql IMMUTABLE AS $$
DECLARE
    nums float8[];
    res  geometry[] := '{}';
    i    int;
    px   float8;
    py   float8;
    lx   float8 := NULL;
    ly   float8 := NULL;
BEGIN
    IF txt IS NULL OR btrim(txt) = '' THEN
        RETURN res;
    END IF;

    SELECT array_agg(v::float8 ORDER BY ord) INTO nums
    FROM regexp_split_to_table(btrim(txt), '[\s,]+') WITH ORDINALITY AS t(v, ord)
    WHERE v ~ '^-?[0-9]+(\.[0-9]+)?$';

    IF nums IS NULL THEN
        RETURN res;
    END IF;

    i := 1;
    WHILE i + 1 <= array_length(nums, 1) LOOP
        px :=  nums[i]     / scale;
        py := -nums[i + 1] / scale;
        IF lx IS NULL OR px <> lx OR py <> ly THEN
            res := res || ST_Point(px, py);
            lx := px;
            ly := py;
        END IF;
        i := i + 2;
    END LOOP;

    RETURN res;
END $$;

-- Соответствие «старый public.nodes.id -> новый net.*.id».
-- Нужно, чтобы линии и дочерние таблицы нашли свои узлы после переноса.
CREATE TABLE IF NOT EXISTS net.node_src_map (
    src_id int PRIMARY KEY,
    id     bigint NOT NULL
);
