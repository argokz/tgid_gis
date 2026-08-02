SELECT obj.id
      ,t1.naimenovanie_lep
      ,t2.znachenie
      ,obj.naimenovanie_priemnika_es
      ,obj.vladelets_priemnika_es
      ,obj.nomer_akta_razdela_es
      ,obj.data_vydachi_akta_razdela_es
      ,obj.marka_transformatora_rp
      ,obj.maksimalno_dopustimaya_nagruzka_vneshnego_vvoda_rp__kvt
      ,obj.trans_pro_kol
      ,obj.trans_sob_kol
      ,obj.edv_pod_kol
      ,obj.edv_obr_kol
      ,obj.edv_dren_kol
      ,obj.edv_opr_kol
      ,obj.diz_kol
      ,obj.chast_reg_reg
      ,obj.gr_pod
  FROM priemnik_elektrosnabzheniya obj
  LEFT JOIN liniya_elektroperedach t1 ON t1.id=obj.naimenovanie_lep
  LEFT JOIN tipy_priemnikov_elektricheskih_setey t2 ON t2.id=obj.typID
