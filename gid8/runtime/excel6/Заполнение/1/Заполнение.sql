SELECT 
  hs.name,  
  months.name,
  
--  lbf.heatSourceID, lbf.monthID, 
  lbf.magistralShare,lbf.distSiteShare, lbf.heatingSystemShare,
  lbf.magistralShare/100*hlm.netWaterFillingNormMS*(lvv.V1+lvv.VobM) AS Gmag,
  lbf.distSiteShare/100*hlm.netWaterFillingNormRS*(lvv.V2+lvv.VobR+lvv.Vpodv) AS Grs,
  lbf.heatingSystemShare/100*hlm.netWaterFillingNormHS*(lvv.Vot+lvv.Vvent+lvv.Vgvs) AS Gtep,
  lbf.netTemperature, lbf.rechargeTemperature,
  
  (SELECT Gmag)*(lbf.netTemperature-lbf.rechargeTemperature)*1E-3,
  (SELECT Grs)*(lbf.netTemperature-lbf.rechargeTemperature)*1E-3 AS Qrs,
  (SELECT Gtep)*(lbf.netTemperature-lbf.rechargeTemperature)*1E-3 AS Qtep


FROM losesByFilling lbf
JOIN losesvolumesview lvv ON lvv.heatSourceID=lbf.heatSourceID
JOIN heatLosesMain hlm ON hlm.id=1
LEFT JOIN heatSources hs ON hs.id=lbf.heatSourceID
LEFT JOIN months ON months.id=lbf.monthID
