SELECT 
  IST.kod_ist,
  IST.name_ist,
  IST.kod,
  IST.uzel,
  pihP-pihO,
  pihP,
  pihO,
  tP,
  tO,
  IST_OUT.t1 AS t1,
  IST_OUT.t2 AS t2

FROM 
  [Источник тепла] IST,
  IST_OUT,
  (
#include Узел.sql
  ) US_OUT0
WHERE
  US_OUT0.kod=IST.kod AND US_OUT0.uzel=IST.uzel
  AND
  IST_OUT.kod=IST.kod AND IST_OUT.uzel=IST.uzel

