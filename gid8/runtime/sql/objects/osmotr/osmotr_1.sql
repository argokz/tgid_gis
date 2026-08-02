SELECT DISTINCT TOP 20000 
      obj.id
      , obj.name
      , obj.data_osmotra
      , obj.nomer_akta
      , obj.predpolagaemye_prichiny_razrusheniya_izolyacii_korrozii
      , obj.rezultaty_osmotra
      , obj.namechennye_meropriyatiya
      , obj.meropriyatiya_po_vosstanovleniyu_prokladki
      , obj.primechanie
      , nach.fio AS otvetstvennoe_lico_ID
      , obj.fio_utverzhdaemogo
--      , dolz1.znachenie AS dolzhnost_utverzhdaemogoID
--      , obj.sluzhba_utverzhdaemogoID
--      , obj.fio_1
--      , obj.dolzhnost_1
--      , obj.fio_2
--      , obj.dolzhnost_2
  FROM osmotr obj
left join osmotrDeployed osd ON osd.directionID = obj.id
JOIN heatPipeSections hpss ON hpss.lineID=osd.lineID $and_condition2$
LEFT JOIN dolzhnosti dolz1 ON dolz1.id=obj.dolzhnost_utverzhdaemogoID
left join nachalniki_uchastkov nach ON nach.id=obj.otvetstvennoe_lico_ID

where  (  obj.data_osmotra $season_condition$  )
