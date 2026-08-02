WITH GroupedChanges AS (
    -- Группированные изменения (change_group_id IS NOT NULL)
    SELECT 
        change_group_id,
        NULL AS log_id,  -- Для групп log_id не отображаем
        NULL AS operation,  -- Аггрегированные, так что отдельные поля не нужны
--        NULL AS table_name,
--        NULL AS record_id,

        (
        SELECT table_name 
         FROM audit_log al 
         WHERE al.change_group_id = audit_log.change_group_id -- and al.table_name in ('nodes', 'linesobj')
         ORDER BY al.log_id
         LIMIT 1
         ) AS table_name,

        (SELECT record_id 
         FROM audit_log al 
         WHERE al.change_group_id = audit_log.change_group_id --  and al.table_name in ('nodes', 'linesobj')
         ORDER BY al.log_id 
         LIMIT 1) AS record_id,


        NULL AS old_data,
        NULL AS new_data,
        NULL AS comment,  -- Комментарий берём из audit_group_comments
        max(changed_at) AS last_changed_at,
        string_agg(changed_by, ', ') AS changed_by,
        bool_and(is_rolled_back) AS all_rolled_back,
        string_agg(
            format('%s on %s (id: %s)%s',
                   operation,
                   table_name,
                   record_id,
                   CASE WHEN comment IS NOT NULL THEN format(', comment: %s', comment) ELSE '' END
            ),
            '; '
        ) AS change_details
    FROM audit_log
    WHERE change_group_id IS NOT NULL
    GROUP BY change_group_id
    HAVING NOT bool_and(is_rolled_back)  -- Исключаем группы, где все изменения откатаны

    UNION ALL

    -- Одиночные изменения (change_group_id IS NULL) - возвращаем все поля для отката
    SELECT 
        NULL AS change_group_id,
        log_id,
        operation,
        table_name,
        record_id,
        old_data,
        new_data,
        comment,  -- Комментарий из audit_log
        changed_at AS last_changed_at,
        changed_by,
        is_rolled_back AS all_rolled_back,
        format('%s on %s (id: %s)%s',
               operation,
               table_name,
               record_id,
               CASE WHEN comment IS NOT NULL THEN format(', comment: %s', comment) ELSE '' END
        ) AS change_details
    FROM audit_log
    WHERE change_group_id IS NULL AND NOT is_rolled_back
)
SELECT 
    gc.log_id, -- 0
    COALESCE(gc.change_group_id::text, '') AS "Группа", -- group_id,  -- 1
    CASE 
        WHEN gc.change_group_id IS NOT NULL THEN COALESCE(agc.comment, '')
        ELSE gc.comment  -- Комментарий из audit_log для одиночных изменений
    END AS  "Комментарий",  -- 2
--    gc.operation,  -- 
    gc.table_name as "Таблица",  -- 3
    gc.record_id as "Номер строки",  -- 4
    case 
    when not n.id is null 
        then ec.name || ' ' || n.externalnodename 
    when not l.id is null 
        then ec1.name || ' ' || n1.externalnodename || ' - ' || ec2.name || ' ' || n2.externalnodename
    else       
        '' end 
    as name,  -- 5

--    ec.name,
--    n.externalnodename,
--    ec1.name,
--    n1.externalnodename,
--    ec2.name,
--    n2.externalnodename,
--    gc.old_data,
--    gc.new_data,
--    gc.change_details,
    gc.last_changed_at as "Дата",  -- 6
    gc.changed_by as "Автор"        -- 7
    , gc.operation  -- 
--    gc.all_rolled_back
FROM GroupedChanges gc
LEFT JOIN audit_group_comments agc ON gc.change_group_id = agc.change_group_id
LEFT JOIN nodes n on n.id=gc.record_id and gc.table_name='nodes'
LEFT JOIN externalcodes ec on n.externalcodeid=ec.id
LEFT JOIN linesobj l on l.id=gc.record_id and gc.table_name='linesobj'
LEFT JOIN nodes n1 on n1.id=l.nodeid1
LEFT JOIN nodes n2 on n2.id=l.nodeid2
LEFT JOIN externalcodes ec1 on n1.externalcodeid=ec1.id
LEFT JOIN externalcodes ec2 on n2.externalcodeid=ec2.id

WHERE table_name IN ('zdaniya_tu', 'tehnicheskie_usloviya')
and gc.changed_by=current_user

ORDER BY gc.last_changed_at DESC
LIMIT 20
    