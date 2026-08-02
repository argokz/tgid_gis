select 
    obj.id,
    obj.name as 'Наименование контура',
    obj.opisaniye_kontura as 'Описание контура',
    ist.naimenovanie as 'Источник тепла',
    ot.name as 'Вид испытания',
    sostoyanie_opres.name as 'Состояние',
    'Утверждение' = IIF(obj.utverdit = 1, 'Утверждено', 'Не утверждено'),
    obj.nodeOprID1 as 'Узел Опрессовочного насоса',
    obj.data_nachala_plan as 'Дата начала опрессовки, план',
    obj.data_okonchaniya_plan as 'Дата окончания опрессовки план',
    obj.davlenie_opressovki_1_etap as 'Давление опрессовки 1 этапа, кгс/см2',
    obj.davlenie_opressovki_2_etap as 'Давление опрессовки 2 этапа, кгс/см2',
    obj.temperatura_raskholazhivaniya_kontura as 'Температура расхолаживания контура, Град.С',
    obj.kolichestvo_zvenjev_obhodchikov as 'Количество звеньев обходчиков',
    obj.fio_rukovoditel_ispytanij as 'ФИО',
    dolzhnosti_ruk.znachenie as 'Должность руководителя испытания',
    subd_ruk.name as 'Подразделение руководителя испытания'
from opres obj
LEFT JOIN dolzhnosti  ON dolzhnosti.id=obj.dolzhnost_rukovoditel_ispytanijID
LEFT JOIN subdivisions subd ON subd.id=obj.podrazdelenie_rukovoditel_ispytanijID
LEFT JOIN istochniki_tepla ist ON ist.id=obj.istochnik_tepla
left join opres_types ot on ot.id = obj.opres_typeID
left join sostoyanie_opres on sostoyanie_opres.id = obj.sostoyanie_opresID
left join nodes n1 on n1.id = obj.nodeOprID1
LEFT JOIN dolzhnosti dolzhnosti_ruk  ON dolzhnosti_ruk.id=obj.dolzhnost_rukovoditel_ispytanijID
LEFT JOIN subdivisions subd_ruk ON subd.id=obj.podrazdelenie_rukovoditel_ispytanijID
where obj.sostoyanie_opresID = 1
AND
  (
      (obj.data_nachala_plan $season_condition$) OR
      (obj.date_opres $season_condition$)
  )
