N_adm_rayon MenuQ "$main$","Выберите район","SELECT DISTINCT adm_rayon AS 'Район' FROM arcgis.dbo.nagruzki WHERE adm_rayon IS NOT NULL AND adm_rayon <> '' ORDER BY adm_rayon",0
N_rayon MenuQ "$main$","Выберите район","SELECT DISTINCT rayon AS 'Район' FROM arcgis.dbo.nagruzki WHERE rayon IS NOT NULL AND rayon <> '' ORDER BY rayon",0
N_uchastok MenuQ "$main$","Выберите участок","SELECT DISTINCT uchastok AS 'Участок района' FROM arcgis.dbo.nagruzki WHERE uchastok IS NOT NULL AND uchastok <> '' ORDER BY uchastok",0
N_ist MenuQ "$main$","Выберите источник","SELECT DISTINCT ist AS 'Источник' FROM arcgis.dbo.nagruzki WHERE ist IS NOT NULL AND ist <> '' ORDER BY ist",0
N_tg MenuQ "$main$","Выберите график","SELECT DISTINCT tg AS 'График' FROM arcgis.dbo.nagruzki WHERE tg IS NOT NULL AND tg <> '' ORDER BY tg",0
N_street MenuQ "$main$","Выберите улицу","SELECT DISTINCT street AS 'Улица' FROM arcgis.dbo.nagruzki WHERE street IS NOT NULL AND street <> '' ORDER BY street",0
N_mkr MenuQ "$main$","Выберите микрорайон","SELECT DISTINCT mkr AS 'Микрорайон' FROM arcgis.dbo.nagruzki WHERE mkr IS NOT NULL AND mkr <> '' ORDER BY mkr",0
