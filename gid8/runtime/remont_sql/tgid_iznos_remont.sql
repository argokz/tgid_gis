SELECT 
  pps.id,
  ec1.name AS kod1, n1.externalNodeName AS uzel1, ec2.name AS kod2, n2.externalNodeName AS uzel2, _T2.cnt, -- _T2.lenP, _T2.lenO,  

  CURDATE() AS date_calc, -- Дата расчета планирования ремонта 
  '??' AS v08 , -- Приоритет ненадежности участка 

  IF(mag.naimenovanie_magistrali IS NULL, rs.naimenovanie_uchastka_rs,mag.naimenovanie_magistrali) AS mag, -- Магистраль/РаспредСеть
  tt.name AS name_tt,   -- Тип прокладки
  tt2.name AS name_tt2, -- Материал трубы

  hps1.diameterCondit, -- Условный  диаметр трубопровода, мм
  if (_T2.lenP > _T2.lenO, _T2.lenP, _T2.lenO) AS len, -- Протяженность участка, м
  
  hps1.firstPICdateHP, -- Дата первичного ввода в эксплуатацию
  hps1.lastTransDate, -- Дата последней перекладки (капитального ремонта)
  25 AS srok, -- Расчетный срок службы, лет
  TIMESTAMPDIFF(YEAR,hps1.firstPICdateHP,(SELECT date_calc)) AS srok2,  -- Срок эксплуатации, лет
  IF((SELECT srok2) <= 7, 1, IF((SELECT srok2) <= 15, 2, IF((SELECT srok2) < 20, 3, IF((SELECT srok2) < 25, 4, 5)))) AS srok2_ball,


  "??" AS v18, -- Количество капитальных ремонтов
  "??" AS v19, -- Количество нарушений (после последней перекладки)
  "??" AS v20, -- Коэффициент удельной повреждаемости
  "??" AS v21, -- Коэфф.уд.повреждаемости за последние 2 года эксплуат.
  "??" AS v22, -- Наличие эл.коммуникаций, электрохимзащиты газопровода 
  "??" AS v23, -- Наличие  электрифицированного транспорта на расстоянии
  "??" AS v24, -- Уровень разности  электропотенциала, В
  "??" AS v25, -- Степень внешней коррозии 
  "??" AS v26, -- Степень внутренней коррозии 
  "??" AS v27, -- Интенсивность подтопления грунтовыми водами
  "??" AS v28, -- Интенсивность подтопления паводковыми водами
  "??" AS v29, -- Интенсивность подтопления водопроводными водами
  "??" AS v30, -- Интенсивность подтопления фекальными водами
  "??" AS v31, -- Состояние строительных конструкций 

  "??" AS v32, -- Водоотведение на участке
  "??" AS v33, -- Сложность отключения участка для устранения повреждения 
  "??" AS v34, -- Объем отключаемых потребителей, при повреждении, Гкал/ч
  "??" AS v35, -- Сложность выполнения аварийно-восстановительных работ
  "??" AS v36, -- Прокладка  трубопроводов под пешеходными зонами
  "??" AS v37, -- Прокладка  трубопроводов под авто и ж/д дорогами
  "??" AS v38, -- Прокладка трубопровода в местах массового скопления жителей города
  "??" AS v39, -- Нанесение   ущерба населению от подтопления при возможном повреждении  трубопроводов 
  "??" AS v40, -- Нанесение   ущерба инфраструктуре города  при возможном повреждении  трубопроводов 


  org.name AS name_org -- Владелец участка

--  hps1.*                                                                                                          
FROM heatPipeSections hps1
LEFT OUTER JOIN heatPipeSections hps2 ON hps1.pipeSectionID = hps2.pipeSectionID AND (hps1.pipeSectLength < hps2.pipeSectLength OR (hps1.pipeSectLength = hps2.pipeSectLength AND hps1.id < hps2.id))
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
left join externalCodes ec1 on ec1.id = n1.externalCodeID
left join externalCodes ec2 on ec2.id = n2.externalCodeID
left join tubingTypes tt on tt.id = hps1.tubingTypeID

JOIN linesobj l2 ON l2.id=hps1.lineID
LEFT JOIN organizations org ON org.id=l2.organizationID

left join tubeTypes tt2 on tt2.id = hps1.tubeTypeID


left join ps.magistrali mag ON mag.id=hps1.magistral
left join ps.uchastok_rs rs ON rs.id=hps1.distSite


WHERE hps2.id IS NULL

ORDER BY pps.id