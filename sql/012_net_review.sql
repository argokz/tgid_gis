-- Хранение альтернативных вариантов объекта и пометка «требует решения».
--
-- Когда на один узел приходится несколько строк подтипа, конвертер
-- выбирает одну (по числу заполненных полей), но вторая НЕ выбрасывается:
-- она сохраняется здесь целиком, а объект помечается как спорный.
-- Пока решение не принято, видно и что выбрано, и что отвергнуто.
--
-- Применяется к уже созданной схеме net, пересборка не требуется.

CREATE TABLE IF NOT EXISTS net.object_variant (
    id         bigserial PRIMARY KEY,
    obj_id     bigint  NOT NULL,          -- id объекта в net
    obj_kind   text    NOT NULL,          -- 'node' | 'line'
    target     text    NOT NULL,          -- целевая таблица объекта
    src_table  text    NOT NULL,          -- исходная таблица подтипа
    src_id     bigint  NOT NULL,          -- id исходной строки
    chosen     boolean NOT NULL,          -- эта строка выбрана конвертером
    score      int,                       -- сколько полей заполнено
    payload    jsonb   NOT NULL           -- полное содержимое строки
);

CREATE INDEX IF NOT EXISTS object_variant_obj_idx
    ON net.object_variant (obj_id);
CREATE INDEX IF NOT EXISTS object_variant_src_idx
    ON net.object_variant (src_table, src_id);

-- Пометка на объекте. Держим в реестре: одно место вместо колонки
-- в каждой из двух десятков объектных таблиц.
ALTER TABLE net.node_reg ADD COLUMN IF NOT EXISTS needs_review boolean NOT NULL DEFAULT false;
ALTER TABLE net.line_reg ADD COLUMN IF NOT EXISTS needs_review boolean NOT NULL DEFAULT false;

CREATE INDEX IF NOT EXISTS node_reg_review_idx
    ON net.node_reg (needs_review) WHERE needs_review;
CREATE INDEX IF NOT EXISTS line_reg_review_idx
    ON net.line_reg (needs_review) WHERE needs_review;

-- Удобный список спорных объектов с обеими версиями рядом.
CREATE OR REPLACE VIEW net.v_needs_review AS
SELECT r.id                                   AS obj_id,
       r.kind                                 AS klass,
       v.src_table,
       count(*)                               AS variantov,
       max(v.score) FILTER (WHERE v.chosen)   AS ball_vybrannogo,
       max(v.score) FILTER (WHERE NOT v.chosen) AS ball_otvergnutogo,
       jsonb_agg(jsonb_build_object(
           'src_id',  v.src_id,
           'chosen',  v.chosen,
           'score',   v.score,
           'payload', v.payload) ORDER BY v.chosen DESC, v.src_id)
                                              AS versii
FROM net.node_reg r
JOIN net.object_variant v ON v.obj_id = r.id AND v.obj_kind = 'node'
WHERE r.needs_review
GROUP BY r.id, r.kind, v.src_table

UNION ALL

SELECT r.id, r.kind, v.src_table, count(*),
       max(v.score) FILTER (WHERE v.chosen),
       max(v.score) FILTER (WHERE NOT v.chosen),
       jsonb_agg(jsonb_build_object(
           'src_id',  v.src_id,
           'chosen',  v.chosen,
           'score',   v.score,
           'payload', v.payload) ORDER BY v.chosen DESC, v.src_id)
FROM net.line_reg r
JOIN net.object_variant v ON v.obj_id = r.id AND v.obj_kind = 'line'
WHERE r.needs_review
GROUP BY r.id, r.kind, v.src_table;

-- Применение принятого решения: сделать выбранным другой вариант.
-- Переписывает поля объекта из сохранённой версии и снимает пометку.
CREATE OR REPLACE FUNCTION net.apply_variant(p_src_id bigint, p_src_table text)
RETURNS void
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $$
DECLARE
    v      net.object_variant%ROWTYPE;
    cols   text;
    reg    text;
BEGIN
    SELECT * INTO v FROM net.object_variant
    WHERE src_id = p_src_id AND src_table = p_src_table;

    IF v.id IS NULL THEN
        RAISE EXCEPTION 'вариант % из % не найден', p_src_id, p_src_table;
    END IF;

    -- Только те поля варианта, которые есть в целевой таблице и не
    -- являются вычисляемыми. Приведение к типу колонки обязательно:
    -- payload ->> отдаёт text, и без него присваивание в числовую
    -- колонку падает.
    SELECT string_agg(
               format('%I = ($1->>%L)::%s',
                      a.attname, a.attname,
                      format_type(a.atttypid, a.atttypmod)), ', ')
    INTO cols
    FROM pg_attribute a
    JOIN pg_class c   ON c.oid = a.attrelid
    JOIN pg_namespace n ON n.oid = c.relnamespace
    WHERE n.nspname = 'net' AND c.relname = v.target
      AND a.attnum > 0 AND NOT a.attisdropped
      AND a.attgenerated = ''
      AND a.attname NOT IN ('id', 'geom', 'fragment_id', 'src_id',
                            'node_from', 'node_to', 'removed_at')
      AND v.payload ? a.attname;

    IF cols IS NOT NULL THEN
        EXECUTE format('UPDATE net.%I SET %s WHERE id = $2', v.target, cols)
        USING v.payload, v.obj_id;
    END IF;

    UPDATE net.object_variant SET chosen = (id = v.id) WHERE obj_id = v.obj_id;

    reg := CASE WHEN v.obj_kind = 'line' THEN 'line_reg' ELSE 'node_reg' END;
    EXECUTE format('UPDATE net.%I SET needs_review = false WHERE id = $1', reg)
    USING v.obj_id;
END $$;

COMMENT ON FUNCTION net.apply_variant(bigint, text) IS
    'Принять отвергнутый вариант как верный: net.apply_variant(15373, ''generalizedconsumers'')';
