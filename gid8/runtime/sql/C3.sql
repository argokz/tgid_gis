SELECT top 2147483647
n.id,
ec.name as 'kod', n.externalNodeName as 'uzel', es.name as 'pr',
twv.structure as 'structure', twv.purpose as 'prz_klan', twv.calcMixFactCoeff as 'u', twv.calcThrustLos as 'dP_pot', twv.expendCharStraight as 'h_1', 
twv.expendCharVert as 'h_2', twv.authority as 'A_treb', twv.calcCapDeviation as 'k_kvs_treb', twv.type as 'tip_klap', twv.capacity as 'kv_s', 
twv.regulator as 'organ', twv.maxPD as 'dP', twv.diameterNomin as 'DN', twv.calcExpendNodeMix as 'Gmax', twv.pumpStationName as 'name_nas_stanzii', 
twv.state as 'sost', o.name as 'operator', n.organizationID as 'Владелец', n.fileID

from threeWayValves twv
join nodes n on n.id = twv.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join operators o on o.id = n.operatorID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
