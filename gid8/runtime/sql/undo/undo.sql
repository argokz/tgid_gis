WITH flagged AS (
  SELECT 
    id,
    changetypeid,
    SUM(CASE WHEN changetypeid = 23 THEN 1 ELSE 0 END) 
      OVER (ORDER BY id) AS block_num
  FROM changed_object
),
bounded AS (
  SELECT 
    f.*, 
    MAX(CASE WHEN changetypeid = 24 THEN 1 ELSE 0 END) 
      OVER (PARTITION BY block_num) AS has_end,
    MIN(CASE WHEN changetypeid = 23 THEN id END)
      OVER (PARTITION BY block_num) AS block_start_id,
    MAX(CASE WHEN changetypeid = 24 THEN id END)
      OVER (PARTITION BY block_num) AS block_end_id
  FROM flagged f
),
final AS (
  SELECT *,
         CASE 
           WHEN has_end = 1 AND id > block_start_id AND id <= block_end_id THEN 'inside'
           WHEN has_end = 1 AND id = block_start_id THEN 'start'
           ELSE 'outside'
         END AS position
  FROM bounded
)
--SELECT id, changetypeid
SELECT 
co.id,co.changetypeid,co.old_id,co.changedid,co.changedtext,co.dateremoved,co.userremoved

FROM final
join changed_object co on co.id=final.id


WHERE position IN ('outside', 'start')
ORDER BY id desc;
