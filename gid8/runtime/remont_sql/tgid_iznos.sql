SELECT 
  id,
  kod1, uzel1, kod2, uzel2, cnt,

  date_calc, -- Дата расчета процента износа 

  procent,  -- Процент износа, % 


  mag, -- Магистраль/РаспредСеть
  name_tt,   -- Тип прокладки
  name_tt2, -- Материал трубы

  diameterCondit, -- Условный  диаметр трубопровода, мм
  len, -- Протяженность участка, м
  
  firstPICdateHP, -- Дата первичного ввода в эксплуатацию
  lastTransDate, -- Дата последней перекладки (капитального ремонта)
  date_osv, -- Дата последнего технического освидетельствования -- дата из точки

  srok, -- Расчетный срок службы, лет
  srok2,  -- Срок эксплуатации, лет

  v19, -- Внешний вид                  1 - отсутствие поверсхностных повреждений 5 - отсутствие поверсхностных повреждений  вес 0.05
  v20, -- Состояние оборудования       1 - новое, почти новое  2 - в работе в неаварийном состоянии 3 в работе в предаварийном состоянии 4 в работе в аварийном состонии 5 не работает вес  0.2
  n_cap, -- Количество капитальных ремонтов   - из точек
  n_def, -- Количество нарушений (после последней перекладки) 
  v23, -- Категории нарушений (текущих ремонтов) 1 незначительные сбои ,  2 не вызывало аврийных ситуаций 4 вызывало нарушение работы сети  5 оборудование физически невозможно вручить в работу вес 0.1

  v24, -- Коэффициент удельной поврежд-и, (кол.повр/ L м)   (v21+м22)/len*100  <1 : 1,  1-2 : 2, 2-4 : 3, 4-5 : 4, > 4 : 5 вес 0.2
  v25, -- Степень внешней коррозии  1 - слабое утонение до 10% 3 - среднее утонение от 10% до 30% 5 - сильное утонение больше 50%  вес 0.1  - толщинометр
  v26, -- Степень внутренней коррозии 1 - допустимое 3 - высокое 5 аварийное вес 0.1  - индикатор коррозии
  v27, -- Состояние строительных конструкций  1 Отклонение от проектных решений, произошедшие в процессе эксплуатации  3 - Снижение несущей способности конструкций  3 Нарушение герметичности стыков  5 -  Излом конструкций перекрытия и стен канала  5 Разрушение неподвижных опор  вес 0.1

  name_org, -- Владелец участка

  koef_povr, -- 0.2
  srok2_ball,  --   вес 0.15
  VnesniiVid2,
  SostOborudovania2,
  Kategorii2,
  VnesnKorrozia2,
  VnunrenKorrozia2,
  SostKonstrukz2,

  IF(procent < 20, '1', 
  IF(procent < 40, '2', 
  IF(procent < 60, '3', 
  IF(procent < 80, '4', 
  '5')))) AS variant


FROM (

    SELECT 
      pps.id,
--      iz.id,
      ec1.name AS kod1, n1.externalNodeName AS uzel1, ec2.name AS kod2, n2.externalNodeName AS uzel2, _T2.cnt, -- _T2.lenP, _T2.lenO,  
      calc.DateRascheta AS date_calc, -- Дата расчета процента износа 

    --  0.2 * IF((SELECT v24) < 1, 1, IF( (SELECT v24) < 2,  2, IF( (SELECT v24) < 4, 3, IF((SELECT v24) < 5, 4, 5)))) AS procent,

    --  "??" AS procent, -- Процент износа, % 


      IF(mag.naimenovanie_magistrali IS NULL, rs.naimenovanie_uchastka_rs,mag.naimenovanie_magistrali) AS mag, -- Магистраль/РаспредСеть
      tt.name AS name_tt,   -- Тип прокладки
      tt2.name AS name_tt2, -- Материал трубы

      hps1.diameterCondit, -- Условный  диаметр трубопровода, мм
      if (_T2.lenP > _T2.lenO, _T2.lenP, _T2.lenO) AS len, -- Протяженность участка, м
      
      hps1.firstPICdateHP, -- Дата первичного ввода в эксплуатацию
      hps1.lastTransDate, -- Дата последней перекладки (капитального ремонта)

      sh1.data_osmotra AS date_osv, -- Дата последнего технического освидетельствования -- дата из точки

      25 AS srok, -- Расчетный срок службы, лет
      TIMESTAMPDIFF(YEAR,hps1.firstPICdateHP,(SELECT date_calc)) AS srok2,  -- Срок эксплуатации, лет

--      iz.VnesniiVid AS v19, -- Внешний вид       1 - отсутствие поверсхностных повреждений 5 - отсутствие поверсхностных повреждений  вес 0.05
      f1.name AS v19, --

--      iz.SostOborudovania AS v20, -- Состояние оборудования       1 - новое, почти новое  2 - в работе в неаварийном состоянии 3 в работе в предаварийном состоянии 4 в работе в аварийном состонии 5 не работает вес  0.2
      f3.name AS v20, 
      
      IFNULL(cap1.n_defekt, 0) AS n_cap, -- Количество капитальных ремонтов   - из точек
      IFNULL(df1.n_defekt, 0) AS n_def, -- Количество нарушений (после последней перекладки) 

      f2.name AS v23,
      
--      '' AS v23, -- Категории нарушений (текущих ремонтов) 1 незначительные сбои ,  2 не вызывало аврийных ситуаций 4 вызывало нарушение работы сети  5 оборудование физически невозможно вручить в работу вес 0.1

      ((SELECT n_cap)+(SELECT n_def))*100/(SELECT len) AS  v24, -- Коэффициент удельной поврежд-и, (кол.повр/ L м)   (v21+м22)/len*100  <1 : 1,  1-2 : 2, 2-4 : 3, 4-5 : 4, > 4 : 5 вес 0.2

      f7.name AS v25,
      f8.name AS v26,
      buildingConstrStates.name AS v27,
      
--      iz.VnesnKorrozia AS v25, -- Степень внешней коррозии  1 - слабое утонение до 10% 3 - среднее утонение от 10% до 30% 5 - сильное утонение больше 50%  вес 0.1  - толщинометр
--      iz.VnunrenKorrozia AS v26, -- Степень внутренней коррозии 1 - допустимое 3 - высокое 5 аварийное вес 0.1  - индикатор коррозии
--      iz.SostKonstrukz AS v27, -- Состояние строительных конструкций  1 Отклонение от проектных решений, произошедшие в процессе эксплуатации  3 - Снижение несущей способности конструкций  3 Нарушение герметичности стыков  5 -  Излом конструкций перекрытия и стен канала  5 Разрушение неподвижных опор  вес 0.1

      org.name AS name_org, -- Владелец участка

    -- 1-5

      IF((SELECT srok2) <= 7, 1, IF((SELECT srok2) <= 15, 2, IF((SELECT srok2) < 20, 3, IF((SELECT srok2) < 25, 4, 5)))) AS srok2_ball,  --   вес 0.15
      IF((SELECT v24) < 1, 1, IF( (SELECT v24) < 2,  2, IF( (SELECT v24) < 4, 3, IF((SELECT v24) < 5, 4, 5)))) AS koef_povr, -- 0.2
      IFNULL(pps.VnesniiVid, 0) AS VnesniiVid2,
      IFNULL(pps.SostOborudovania, 0) AS SostOborudovania2,
      IFNULL(pps.Kategorii, 0) AS Kategorii2,
      IFNULL(pps.VnesnKorrozia, 0) AS VnesnKorrozia2,
      IFNULL(pps.VnunrenKorrozia, 0) AS VnunrenKorrozia2,
      IFNULL(pps.SostKonstrukz, 0) AS SostKonstrukz2,


      ((SELECT srok2_ball)*0.15+
      (SELECT koef_povr)*0.2+
      (SELECT VnesniiVid2)*0.05+
      (SELECT SostOborudovania2)*0.2+
      (SELECT Kategorii2)*0.1+
      (SELECT VnesnKorrozia2)*0.1+
      (SELECT VnunrenKorrozia2)*0.1+
      (SELECT SostKonstrukz2)*0.1)*20 AS procent

    --  hps1.*                                                                                                          
    FROM heatPipeSections hps1
    LEFT OUTER JOIN heatPipeSections hps2 ON hps1.pipeSectionID = hps2.pipeSectionID AND NOT hps1.pipeSectionID IS NULL  AND NOT hps2.pipeSectionID IS NULL AND (hps1.pipeSectLength < hps2.pipeSectLength OR (hps1.pipeSectLength = hps2.pipeSectLength AND hps1.id < hps2.id))
    JOIN
    (
    SELECT 
      hps2.pipeSectionID, 
      COUNT(hps2.id) cnt,
      SUM(IF (l.externalSignLineID IN (1,2,4), hps2.pipeSectLength, 0)) AS lenP,
      SUM(IF (l.externalSignLineID IN (1,3,5), hps2.pipeSectLength, 0)) AS lenO 

      FROM heatPipeSections hps2
      JOIN linesobj l ON l.id=hps2.lineID
      GROUP BY hps2.pipeSectionID
    ) _T2 ON _T2.pipeSectionID = hps1.pipeSectionID
    JOIN pipeSections pps ON pps.id=hps1.pipeSectionID
    JOIN nodes n1 ON n1.id=pps.nodeID1
    JOIN nodes n2 ON n2.id=pps.nodeID2
    LEFT JOIN externalCodes ec1 on ec1.id = n1.externalCodeID
    LEFT JOIN externalCodes ec2 on ec2.id = n2.externalCodeID
    LEFT JOIN tubingTypes tt on tt.id = hps1.tubingTypeID

    JOIN linesobj l2 ON l2.id=hps1.lineID
    LEFT JOIN organizations org ON org.id=l2.organizationID

    LEFT JOIN tubeTypes tt2 on tt2.id = hps1.tubeTypeID


    LEFT JOIN ps.magistrali mag ON mag.id=hps1.magistral
    LEFT JOIN ps.uchastok_rs rs ON rs.id=hps1.distSite

    LEFT JOIN 
    (
       SELECT 
       hps1.pipeSectionID, 
       MAX(s1.data_osmotra) AS data_osmotra,
       COUNT(s1.id)
       FROM ps.shurf2 s1
       LEFT JOIN linesobj l1 ON l1.id=s1.lineID
       LEFT JOIN heatPipeSections hps1 ON hps1.lineID=l1.id
       GROUP BY hps1.pipeSectionID
    ) sh1 ON sh1.pipeSectionID=pps.id


    LEFT JOIN 
    (
      SELECT 
         hps1.pipeSectionID, 
         MAX(s1.data_osmotra) AS data_osmotra,
         COUNT(s1.id) AS n_defekt
         FROM ps.defekt2 s1
         LEFT JOIN linesobj l1 ON l1.id=s1.lineID
         LEFT JOIN heatPipeSections hps1 ON hps1.lineID=l1.id
         GROUP BY hps1.pipeSectionID
    ) df1 ON df1.pipeSectionID=pps.id

    LEFT JOIN 
    (
      SELECT 
         hps1.pipeSectionID, 
         MAX(s1.data_osmotra) AS data_osmotra,
         COUNT(s1.id) AS n_defekt
         FROM ps.capital2 s1
         LEFT JOIN linesobj l1 ON l1.id=s1.lineID
         LEFT JOIN heatPipeSections hps1 ON hps1.lineID=l1.id
         GROUP BY hps1.pipeSectionID
    ) cap1 ON cap1.pipeSectionID=pps.id

    LEFT JOIN ps.iznos iz ON iz.pipeSectionID=pps.id
    LEFT JOIN ps.CALCULATION_IZNOS calc ON calc.id=iz.calculationID --  AND calc.DateRascheta='$DATE$'

    LEFT JOIN faktIznos_1_vneshnii_vid f1 ON f1.id=pps.VnesniiVid
    LEFT JOIN faktIznos_3_sost_obor f3 ON f3.id=pps.SostOborudovania
    LEFT JOIN faktIznos_2_kat_narush f2 ON f2.id=pps.Kategorii

    LEFT JOIN faktRiska_7_vneshkorroz f7 ON f7.id=pps.VnesnKorrozia
    LEFT JOIN faktRiska_8_vnutkorroz f8 ON f8.id=pps.VnunrenKorrozia
    LEFT JOIN buildingConstrStates ON buildingConstrStates.id=pps.SostKonstrukz

    WHERE hps2.id IS NULL
) _TTTTT


ORDER BY id
