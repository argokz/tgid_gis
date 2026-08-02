SELECT p.id, m.naimenovanie_magistrali AS "Наименование магистрали", 
 ec1.name AS "Код РС начальной камеры", nn1.externalNodeName AS "Наименование начальной камеры", 
 ec2.name AS "Код РС конечной камеры", nn2.externalNodeName AS "Наименование конечной камеры",

p.magistral,
m.naimenovanie_magistrali AS __magistral,
p.magistralSite,
p.distSite,
p.tubingTypeID,
tubingTypes.name AS name1,
p.firstPICdateHP,
p.lastTransDate,
p.lastIsolDate,
p.DiamUslov,

p.pipeLength,

p.standart,
p.inventNumber,
tubeTypes.name,
p.objectTypeID,
p.Sreda,
coolantTypes.name AS __Sreda,
p.press,
p.temperature,
p.SpokSluzhbiRash,
p.ressurs,
p.pusk,
p.organizationID,
o.name AS __organizationID,

p.Elektrich,
f4.name AS name01,
p.TransportElekricht,
f5.name AS name02,
p.Ponezial,
f6.name AS name03,

p.floodIntensityGWid,
floodIntensitiesGW.name AS name04,
p.floodIntensityFWid,
floodIntensitiesFW.name AS name05,
p.floodIntensityTWid,
floodIntensitiesTW.name AS name06,
p.floodIntensityBWid,
floodIntensitiesBW.name AS name07,

p.Vodootved,
sectWaterDumps.name AS name08,
p.SlozhOkluzh,
breakComplexities.name AS name09,
f13.name, -- p.OtkluzhGkal,
p.AvariiVipoln,
ERWdifficulties.name AS name10,
p.Pesehod,
f15.name AS name11,
p.ZhdDorogi,
tubingWays.name AS name12,
p.LudiProklad,
f17.name AS name13,
p.UzherbLudi,
populDamages.name AS name14,
p.UzherbSity,
infrastrDamages.name AS name15,
p.VnesniiVid,
f1.name AS name16,
p.SostOborudovania,
f3.name AS name17,
p.VnesnKorrozia,
f7.name AS name18,
p.VnunrenKorrozia,
f8.name AS name19,
p.SostKonstrukz,
buildingConstrStates.name AS name20,
f2.name


FROM pipeSections p 
JOIN nodes nn1 ON nn1.id=p.nodeID1 
JOIN nodes nn2 ON nn2.id=p.nodeID2 
join externalCodes ec1 on ec1.id = nn1.externalCodeID 
join externalCodes ec2 on ec2.id = nn2.externalCodeID 
JOIN magistrali m ON m.id=p.magistral
LEFT JOIN organizations o ON o.id=p.organizationID 
LEFT JOIN tubeTypes ON tubeTypes.id=p.tubeTypeID


LEFT JOIN tubingTypes ON tubingTypes.id=p.tubingTypeID
LEFT JOIN floodIntensitiesGW ON floodIntensitiesGW.id=p.floodIntensityGWid
LEFT JOIN floodIntensitiesFW ON floodIntensitiesFW.id=p.floodIntensityFWid
LEFT JOIN floodIntensitiesTW ON floodIntensitiesTW.id=p.floodIntensityTWid
LEFT JOIN floodIntensitiesBW ON floodIntensitiesBW.id=p.floodIntensityBWid
LEFT JOIN sectWaterDumps ON sectWaterDumps.id=p.Vodootved
LEFT JOIN breakComplexities ON breakComplexities.id=p.SlozhOkluzh
LEFT JOIN ERWdifficulties ON ERWdifficulties.id=p.AvariiVipoln
LEFT JOIN tubingWays ON tubingWays.id=p.ZhdDorogi
LEFT JOIN populDamages ON populDamages.id=p.UzherbLudi
LEFT JOIN infrastrDamages ON infrastrDamages.id=p.UzherbSity
LEFT JOIN buildingConstrStates ON buildingConstrStates.id=p.SostKonstrukz
LEFT JOIN coolantTypes ON coolantTypes.id=p.Sreda
LEFT JOIN objectTypes2 ON objectTypes2.id=p.objectTypeID


LEFT JOIN faktIznos_1_vneshnii_vid f1 ON f1.id=p.VnesniiVid
LEFT JOIN faktIznos_3_sost_obor f3 ON f3.id=p.SostOborudovania
LEFT JOIN faktRiska_4_elekt f4 ON f4.id=p.Elektrich
LEFT JOIN faktRiska_5_transport f5 ON f5.id=p.TransportElekricht
LEFT JOIN faktRiska_6_potencial f6 ON f6.id=p.Ponezial
LEFT JOIN faktRiska_12_slozhnost_otkl f12 ON f12.id=p.SlozhOkluzh
LEFT JOIN faktRiska_15_peshehod f15 ON f15.id=p.Pesehod
LEFT JOIN faktRiska_17_skoplenie_zhit f17 ON f17.id=p.LudiProklad

LEFT JOIN faktRiska_7_vneshkorroz f7 ON f7.id=p.VnesnKorrozia
LEFT JOIN faktRiska_8_vnutkorroz f8 ON f8.id=p.VnunrenKorrozia

LEFT JOIN faktRiska_13_otkl_potrebiteli f13 ON f13.id=p.OtkluzhGkal

LEFT JOIN faktIznos_2_kat_narush f2 ON f2.id=p.Kategorii


ORDER BY p.id


-- LEFT JOIN tubeTypes ON tubeTypes.id=p.tubeTypeID

