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
LANGUAGE plpgsql IMMUTABLE
SET search_path = pg_catalog, public AS $$
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

-- Обратное преобразование: геометрия -> текстовое поле coords в том виде,
-- в каком его ожидает приложение (CCoordList::loadStr).
--
-- Возвращаются ТОЛЬКО промежуточные вершины: первая и последняя точки линии —
-- это узлы, и приложение подставляет их само. Единицы — сантиметры,
-- ось Y инвертирована обратно.
-- SET search_path обязателен: функция используется в вычисляемой колонке
-- coords_legacy, а pg_restore выполняет такие выражения с урезанным
-- search_path и не находит ST_DumpPoints — восстановление БД падает.
CREATE OR REPLACE FUNCTION net.geom_to_coords(g geometry, scale float8 DEFAULT 100.0)
RETURNS text
LANGUAGE sql IMMUTABLE
SET search_path = pg_catalog, public AS $$
    -- trim_scale убирает хвостовые нули, но НЕ округляет до целых:
    -- в исходных coords встречаются доли сантиметра, и округление
    -- сделало бы преобразование туда-обратно неточным.
    SELECT coalesce(string_agg(
               trim_scale(round((ST_X(p.geom) * scale)::numeric, 4))::text || ' ' ||
               trim_scale(round((-ST_Y(p.geom) * scale)::numeric, 4))::text || ',',
               '' ORDER BY p.path[1]), '')
    FROM ST_DumpPoints(g) p
    WHERE p.path[1] > 1 AND p.path[1] < ST_NPoints(g);
$$;

-- Соответствие «старый public.nodes.id -> новый net.*.id».
-- Нужно, чтобы линии и дочерние таблицы нашли свои узлы после переноса.
CREATE TABLE IF NOT EXISTS net.node_src_map (
    src_id int PRIMARY KEY,
    id     bigint NOT NULL
);
