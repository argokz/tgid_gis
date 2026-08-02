


SELECT * 
FROM heatPipeSections WHERE (pipeSectionID,pipeSectLength) IN 
( 
SELECT pipeSectionID, MAX(pipeSectLength)
  FROM heatPipeSections
  GROUP BY pipeSectionID
)

ORDER BY pipeSectionID