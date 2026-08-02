SELECT top 2147483647 
  n.id,
  ec.name as 'kod', n.externalNodeName as 'uzel', 
--  es.name as 'pr', 
--  calc.name, 
  b3, cxema, otoplz, otopln, ventil, kondiz, gvop, gvoo, gvpr, gvsm, gvps, gvpw, b4, b5, 
  b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17, b18, b19, b20, b21, b22, b23, 
  b24, b25, b26, b27, b28, b29, b30, b31, b32, b33, b34, b35, b36, b37, b38, b39, b40, 
  b41, b42, b43, diam_P, diam_O, balans, comment
--  , pr_videlen
from DR_OUT
join nodes n on
n.id = dr_out.nodeID
join CALCULATION calc on
calc.id = dr_out.calculationID
join externalCodes ec on
ec.id = n.externalCodeID
join externalSigns es on
es.id = n.externalSignID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND calculationID=$calculationID$
AND n.removed=0 

