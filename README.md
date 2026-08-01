# tgid_gis

Перевод БД и приложения ТГИД с реляционной модели «надтип + подтипы» на ГИС-модель
**«одна таблица — один объект»**, пригодную для прямой работы в QGIS и других ГИС.

## Цели

1. Каждый тип объекта тепловой сети — отдельная таблица со своей геометрией PostGIS.
2. Реальные внешние ключи, индексы, ограничения — целостность на уровне БД, а не приложения.
3. Совместимость: существующее приложение ТГИД продолжает работать во время и после миграции.
4. Скорость: убрать мега-джойны на 26 таблиц при каждом открытии схемы.

**Полная переписка (продукт без compat):** [docs/11-full-rewrite.md](docs/11-full-rewrite.md) —
чистая БД `net`+`ref` для QGIS/ArcGIS и новое приложение. Сборка: `tools/build_clean_gis_db.ps1`.

## Состояние

| Шаг | Что | Статус |
|---|---|---|
| 1 | Обследование БД, базлайн производительности | готово — [01-analysis-current-db.md](docs/01-analysis-current-db.md) |
| 2 | Проект целевой модели, DDL, тесты целостности | готово — [02-target-model.md](docs/02-target-model.md) |
| 3 | Классификация типов объектов по измерениям | готово |
| 4 | Конвертер `public` → `net` | готово — [converter/convert.py](converter/convert.py) |
| 5 | Валидация переноса | готово — [converter/validate.py](converter/validate.py) |
| 6 | Новые запросы чтения, замеры | готово — [03-performance.md](docs/03-performance.md) |
| 7 | Готовность слоёв для QGIS, починка геометрии | готово |
| 8 | Слой совместимости, переключение и откат | готово — [04-compat-layer.md](docs/04-compat-layer.md) |
| 9 | Путь записи через `INSTEAD OF`, смена класса объекта | готово |
| 10 | Новая БД `tgid_gis` на целевой схеме | готово — [tools/build_new_db.ps1](tools/build_new_db.ps1) |
| 11 | Развёртывание запросов в приложение | готово — [tools/deploy_sql.ps1](tools/deploy_sql.ps1) |
| 12 | Правки C++, сборка приложения | готово — Qt 6.8.3, `gid8.exe`, проверка на `tgid_gis` |
| 13 | «Тонкое» представление для отрисовки карты | готово — [sql/070_map_thin.sql](sql/070_map_thin.sql), замер [map_thin_bench.json](docs/schema/map_thin_bench.json) |
| 14 | Развёртывание SQL + aqt при закрытом download.qt.io | готово |
| 15 | Сборка и проверка приложения на новой БД | готово |
| 16–17 | Сироты оборудования / «дубли» справочников | готово — [05](docs/05-orphan-equipment.md), [06](docs/06-duplicate-tables.md) |
| 18 | Выбор строки у дублей по данным; защита от чтения из view | готово |
| 19 | Спорные объекты: обе версии + `needs_review` | готово — [spornye_obekty.sql](sql/queries/spornye_obekty.sql) |
| 20 | Подтипы тоже на `net` (views + INSTEAD OF) | готово — приложение работает полностью |
| 21 | Регрессия гидравлического расчёта (sety) | **structural_ok** на фр. 2 и 1; фикс `regulator_press` — [09](docs/09-calc-regression-results.md) |
| 22 | Разбор спорных потребителей глазами | очередь — [07](docs/07-duplicates-next.md) |
| 23 | Новый Qt-клиент: карта, карточка и безопасное редактирование | готово — [tgid_app](tgid_app) |
| 24–26 | История, архив, создание объектов и топология Qt/QGIS | готово |
| 27 | Реестр функций и SQL старого ТГИД | готово — [12](docs/12-legacy-function-inventory.md) |
| 28 | Справочники `ref` и метаданные карточек Qt | готово — [098](sql/098_core_references.sql), [105](sql/105_ui_field_catalog.sql) |
| 29 | Полный каталог полей и типизированные редакторы Qt | готово — [106](sql/106_complete_field_catalog.sql) |
| 30 | Атомарное разрезание трубопровода из Qt | готово — [описание](docs/13-line-split.md), [110](sql/110_line_split.sql), [тест](sql/tests/test_line_split.sql) |
| 31 | Безопасное соединение двух трубопроводных участков из Qt | готово — [описание](docs/14-line-join.md), [111](sql/111_line_join.sql), [тест](sql/tests/test_line_join.sql) |
| 32 | Атомарное массовое редактирование объектов из Qt | готово — [описание](docs/15-batch-edit.md) |
| — | 16 PR без концов | не в net (orphan) — [10](docs/10-orphan-pressregulators.md) |

## Что дальше

1. **Спорные потребители** — решение заказчика: [07](docs/07-duplicates-next.md),
   [spornye_obekty.sql](sql/queries/spornye_obekty.sql).
2. **Фрагменты без строки в `fragments`** (80, 91, …) — завести id или не считать рабочими.
3. **Сходимость на фр. 1** — крупные Δ температур у виртуальных узлов; не structural.
4. **Функциональная матрица** — утвердить обязательные команды из
   [инвентаризации](docs/12-legacy-function-inventory.md).
5. **Чистый расчётный контур** — схема `calc`, запись результатов без
   legacy-таблиц `public.*_out` и запуск из нового Qt-клиента.

Инвентаризация старого интерфейса и SQL воспроизводится командой:

```bash
python tools/inventory_legacy.py
```

Полный машинный реестр: [legacy_inventory.json](docs/schema/legacy_inventory.json).

## Новая БД и запуск приложения на ней

```bash
$env:PGPASSWORD='...'; .\tools\build_new_db.ps1 -Source almatygid -Target tgid_gis
psql -d tgid_gis -f sql/040_switch_to_net.sql   # nodes/linesobj -> представления
psql -d tgid_gis -f sql/050_write_triggers.sql  # запись через представления
psql -d tgid_gis -f sql/055_subtype_views.sql   # views подтипов в net
psql -d tgid_gis -f sql/060_switch_subtypes.sql # public.*_subtype -> view
psql -d tgid_gis -f sql/070_map_thin.sql        # тонкая карта
.\tools\deploy_sql.ps1                          # новые us.sql / ut.sql в приложение
```

Откат: `sql/041_rollback_to_public.sql`, `sql/061_rollback_subtypes.sql`,
`.\tools\deploy_sql.ps1 -Rollback`.

## Сборка приложения

Окружение на машине разработчика: MSVC и CMake уже входят в Visual Studio 2022.
Дополнительно нужны только Qt 6 и заголовки Boost — PROJ не требуется, `proj.h`
подключается, но ни одна функция PROJ API не вызывается.

```bash
.\tools\build_gid8.ps1
```

Если `download.qt.io` недоступен, Qt ставится через зеркало с настройками
из [tools/aqt_settings.ini](tools/aqt_settings.ini).

## Достигнуто

**32 ГИС-слоя, 440 486 объектов** — каждый тип в своей таблице, с геометрией PostGIS,
первичным ключом и пространственным индексом. Открываются в QGIS напрямую.

**Скорость** (открытие одного фрагмента): линии **5.1x**, узлы **2.2x**.
Схема фрагмента целиком: было ~660 мс, стало ~210 мс.
Тонкая карта (шаг 13, фрагмент 80): узлы **4.2x**, линии **2.1x** относительно
полных `us_net`/`ut_net` — см. [map_thin_bench.json](docs/schema/map_thin_bench.json).

**Целостность**: 45 внешних ключей в `net` против 0 в исходной БД.

**Геометрия восстановлена**: было видно 53 966 узлов и 45 917 линий,
стало 91 490 и 86 016.

**Совместимость подтипов**: 18 таблиц-подтипов — представления над `net` с записью
через `INSTEAD OF`; приложение создаёт типизированные объекты без потери класса.

## Порядок применения

```bash
psql -f sql/005_net_functions.sql      # функции разбора координат
psql -f sql/010_net_schema.sql         # схема net (создаётся рядом с public)
python converter/convert.py --mapping converter/mapping.json          # пробный прогон
python converter/convert.py --mapping converter/mapping.json --apply  # запись
python converter/validate.py --mapping converter/mapping.json         # проверки
psql -f sql/020_fix_geometry.sql       # необязательно: починка геометрии из источника
psql -f sql/070_map_thin.sql           # тонкие представления карты для Qt
psql -f sql/080_concurrency.sql        # row_version и защита совместного редактирования
psql -f sql/090_object_history.sql     # общий журнал Qt/QGIS и архивирование
psql -f sql/095_object_insert_history.sql # аудит новых объектов
psql -f sql/096_line_topology.sql       # единая топология линий Qt/QGIS
psql -f sql/097_node_topology.sql       # перенос концов линий при перемещении узлов
psql -f sql/098_core_references.sql      # справочники ref; запускать, пока доступен public
psql -f sql/100_gis_catalog.sql        # каталог слоёв QGIS/Qt и версия схемы
psql -f sql/105_ui_field_catalog.sql    # подписи, единицы и справочные редакторы Qt
psql -f sql/106_complete_field_catalog.sql # все поля и типизированные редакторы Qt
psql -f sql/110_line_split.sql          # атомарное разрезание трубопровода
psql -f sql/111_line_join.sql           # безопасное соединение двух участков
```

Откат: `DROP SCHEMA net CASCADE;` — схема `public` не изменяется ни на одном шаге
до `040_switch_to_net.sql`.

`098_core_references.sql` относится к сборочному контуру: он копирует необходимые
справочники из исходной `public` в `ref` до создания чистого продуктового дампа.

## Подключение

Параметры берутся из переменных окружения, см. [.env.example](.env.example).
Пароли в репозиторий не коммитятся. Локально: скопировать в `.env`.

```bash
cp .env.example .env   # заполнить
python tools/dump_schema.py --out docs/schema/almatygid.json
python tools/analyze_schema.py --schema docs/schema/almatygid.json --out docs/schema/tables.md
```

## Требования

- PostgreSQL 16 + PostGIS 3.5
- Python 3.10+, `psycopg2`
- Для расчёта: venv sety (`H:\venv\sety`) + ODBC PostgreSQL + `gid8/python/sety`
