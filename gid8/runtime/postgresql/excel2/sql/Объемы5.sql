SELECT 
  1 AS period,
  lengthZ AS len,
  VenZ AS Ven,
  VprZ AS Vpr,
  VbpZ AS Vbp,
  VmgZ AS Vmg,
  Vpr+Vbp+Vmg AS Vdrug,
  Ven+Vpr+Vbp+Vmg AS Vuch,

  Vprib AS Vprib1,
  Vbezpr AS Vbezpr1,
  Vprib1+Vbezpr1 AS Vpot,
  Vuch+Vpot AS Vall,

  kod_ist
FROM 
(
#include ־בתולû.sql
) OB,
TEMP_PROD_OUT

UNION

SELECT 
  0,
  lengthL AS len,
  VenL AS Ven,
  VprL AS Vpr,
  VbpL AS Vbp,
  VmgL AS Vmg,
  Vpr+Vbp+Vmg AS Vdrug,
  Ven+Vpr+Vbp+Vmg AS Vuch,

  Vprib AS Vprib1,
  Vbezpr AS Vbezpr1,
  Vprib1+Vbezpr1 AS Vpot,
  Vuch+Vpot AS Vall,

  kod_ist
FROM 
(
#include ־בתולû.sql
) OB,
TEMP_PROD_OUT
