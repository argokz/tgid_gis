SELECT
  O1.kod_ist AS kod_ist,
  VenZ,
  VenL,
  VprZ,
  VprL,
  VbpZ,
  VbpL,
  VmgZ,
  VmgL,
  Vprib,
  Vbezpr,
  QObezpr

FROM
(
#include Объемы участков.sql
) O1,
(
#include Объемы потребителей.sql
) O2

WHERE 
 O1.kod_ist = O2.kod_ist