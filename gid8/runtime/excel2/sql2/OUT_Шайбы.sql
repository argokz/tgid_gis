SELECT top 2147483647 
  1,DR_OUT.uzel,
  b3,
  round(otoplz+otopln,4),
  IIF(ISNULL(a4),0,a4)+IIF(ISNULL(a5),0,a5),
  round(ventil+kondiz,4),
  IIF(ISNULL(a6),0,a6)+IIF(ISNULL(a7),0,a7),
  round(gvop+gvoo+gvpr+gvsm+gvps+gvpw,4),
  IIF(ISNULL(a11),0,a11)+IIF(ISNULL(a12),0,a12)+IIF(ISNULL(a13),0,a13),
  round(b6,1),
  round(b34,1),
  round(b38,1),
  3,
  round(b8,1),
  round(b7,1),
  round(b36,1)
  
FROM 
  DR_OUT,
  PT_OUT
WHERE
  DR_OUT.kod=PT_OUT.kod
AND
  DR_OUT.uzel=PT_OUT.uzel

