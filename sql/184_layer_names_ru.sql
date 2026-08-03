-- Русские названия слоёв в каталоге meta.layer_catalog.
--
-- Столбец display_name_ru существовал с самого начала, но был заполнен
-- копией латинского имени таблицы: каталог заведён, а подписи в нём
-- не было. Здесь он наполняется по-настоящему.
--
-- Названия взяты из терминологии самой программы, а не переведены
-- со схемы: так эти объекты называются в интерфейсе ТГИД и в отчётах
-- (см. cxema/cxema1.cpp, gidview/analiz.cpp). Например net.pipe_section
-- в приложении всегда «участок трубопровода», а не «секция трубы».
--
-- Имена таблиц НЕ меняются: решение заказчика от 3 августа — русскими
-- делаем только подписи. Латинские идентификаторы остаются, поэтому
-- весь SQL в gid8, sety, converter, инструментах и 29 тестов продолжает
-- работать без единой правки.

BEGIN;

UPDATE meta.layer_catalog SET display_name_ru = v.ru
FROM (VALUES
    ('air_heater',                'Калориферы'),
    ('armature_control',          'Регулирующая арматура'),
    ('building',                  'Здания'),
    ('building_consumer_area',    'Здания потребителей (контуры)'),
    ('building_consumer_line',    'Здания потребителей (линии)'),
    ('building_consumer_point',   'Здания потребителей (точки)'),
    ('building_inlet',            'Вводы в здание'),
    ('building_inlet_line',       'Вводы в здание (линии)'),
    ('building_tu_area',          'Здания по техусловиям (контуры)'),
    ('building_tu_line',          'Здания по техусловиям (линии)'),
    ('bypass',                    'Байпасы'),
    ('connect_node',              'Присоединительные узлы'),
    ('consumer_general',          'Обобщённые потребители'),
    ('consumer_real',             'Реальные потребители'),
    ('damper',                    'Шайбы'),
    ('diameter_change',           'Переходы диаметра'),
    ('diaphragm',                 'Диафрагмы'),
    ('elevator',                  'Элеваторы'),
    ('heat_chamber',              'Тепловые камеры'),
    ('heat_exchanger',            'Теплообменники'),
    ('heat_source',               'Источники теплоснабжения'),
    ('line_plain',                'Линии без оборудования'),
    ('local_resistance',          'Местные сопротивления'),
    ('manhole',                   'Колодцы'),
    ('node_plain',                'Узлы без оборудования'),
    ('pipe_reconstructed_line',   'Реконструируемые трубопроводы (линии)'),
    ('pipe_reconstructed_point',  'Реконструируемые трубопроводы (точки)'),
    ('pipe_section',              'Участки трубопроводов'),
    ('pipe_turn',                 'Углы поворота трубопровода'),
    ('pressure_test_defect',      'Дефекты опрессовки'),
    ('pump',                      'Насосы'),
    ('pump_station',              'Насосные станции'),
    ('radiator',                  'Системы отопления'),
    ('refill_node',               'Узлы подпитки'),
    ('regulator_consumption',     'Регуляторы расхода'),
    ('regulator_press',           'Регуляторы давления'),
    ('regulator_pressdrop',       'Регуляторы перепада давления'),
    ('repair_section',            'Участки ремонта'),
    ('support',                   'Опоры'),
    ('valve_3way',                'Трёхходовые клапаны'),
    ('valve_reverse',             'Обратные клапаны')
) AS v(tbl, ru)
WHERE meta.layer_catalog.table_name = v.tbl;

COMMIT;

-- Контроль: не осталось ли слоя, у которого подпись всё ещё совпадает
-- с латинским именем таблицы (значит, его забыли в списке выше).
SELECT count(*) AS vsego,
       count(*) FILTER (WHERE display_name_ru = table_name) AS bez_russkoy_podpisi
FROM meta.layer_catalog;
