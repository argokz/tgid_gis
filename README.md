# tgid_gis

Перевод БД и приложения ТГИД с реляционной модели «надтип + подтипы» на ГИС-модель
**«одна таблица — один объект»**, пригодную для прямой работы в QGIS и других ГИС.

## Цели

1. Каждый тип объекта тепловой сети — отдельная таблица со своей геометрией PostGIS.
2. Реальные внешние ключи, индексы, ограничения — целостность на уровне БД, а не приложения.
3. Совместимость: существующее приложение ТГИД продолжает работать во время и после миграции.
4. Скорость: убрать мега-джойны на 26 таблиц при каждом открытии схемы.

## Состояние

| Шаг | Что | Статус |
|---|---|---|
| 1 | Обследование БД, базлайн производительности | готово — [01-analysis-current-db.md](docs/01-analysis-current-db.md) |
| 2 | Проект целевой модели, DDL, тесты целостности | готово — [02-target-model.md](docs/02-target-model.md) |
| 3 | Классификация типов объектов по измерениям | готово |
| 4 | Конвертер `public` → `net` | готово — [converter/convert.py](converter/convert.py) |
| 5 | Валидация переноса (15 проверок) | готово — [converter/validate.py](converter/validate.py) |
| 6 | Новые запросы чтения, замеры | готово — [03-performance.md](docs/03-performance.md) |
| 7 | Готовность слоёв для QGIS, починка геометрии | готово |
| 8 | Слой совместимости, переключение и откат | готово — [04-compat-layer.md](docs/04-compat-layer.md) |
| 9 | Путь записи через `INSTEAD OF`, смена класса объекта | готово |
| 10 | Новая БД `tgid_gis` на целевой схеме | готово — [tools/build_new_db.ps1](tools/build_new_db.ps1) |
| 11 | Развёртывание запросов в приложение | готово — [tools/deploy_sql.ps1](tools/deploy_sql.ps1) |
| 12 | Правки C++, сборка приложения | в работе |
| 13 | «Тонкое» представление для отрисовки карты | — |

## Новая БД и запуск приложения на ней

```bash
$env:PGPASSWORD='...'; .\tools\build_new_db.ps1 -Source almatygid -Target tgid_gis
psql -d tgid_gis -f sql/040_switch_to_net.sql   # nodes/linesobj -> представления
psql -d tgid_gis -f sql/050_write_triggers.sql  # запись через представления
.\tools\deploy_sql.ps1                          # новые us.sql / ut.sql в приложение
```

Откат на каждом шаге: `sql/041_rollback_to_public.sql` и `.\tools\deploy_sql.ps1 -Rollback`.

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

**Целостность**: 43 внешних ключа против 0 в исходной БД.

**Геометрия восстановлена**: было видно 53 966 узлов и 45 917 линий,
стало 91 490 и 86 016.

## Порядок применения

```bash
psql -f sql/005_net_functions.sql      # функции разбора координат
psql -f sql/010_net_schema.sql         # схема net (создаётся рядом с public)
python converter/convert.py --mapping converter/mapping.json          # пробный прогон
python converter/convert.py --mapping converter/mapping.json --apply  # запись
python converter/validate.py --mapping converter/mapping.json         # проверки
psql -f sql/020_fix_geometry.sql       # необязательно: починка геометрии из источника
```

Откат: `DROP SCHEMA net CASCADE;` — схема `public` не изменяется ни на одном шаге.

## Подключение

Параметры берутся из переменных окружения, см. [.env.example](.env.example).
Пароли в репозиторий не коммитятся.

```bash
cp .env.example .env   # заполнить
python tools/dump_schema.py --out docs/schema/almatygid.json
python tools/analyze_schema.py --schema docs/schema/almatygid.json --out docs/schema/tables.md
```

## Требования

- PostgreSQL 16 + PostGIS 3.5
- Python 3.10+, `psycopg2`
