# Результаты регрессии расчёта: almatygid vs tgid_gis

## Вердикт

| Фрагмент | Структура | Числа | Примечание |
|---|---|---|---|
| **2** | **OK** | внутри шума (~497 на `a13`) | эталон после фикса `regulator_press` |
| **1** | **OK** | крупные Δ на температурах / части `a13` | нестабильность решателя на магистрали, не дыры id |
| **80** | — | — | **нет строки в `fragments`** → sety: «Какая-то ошибка!!!!» |

Критерий «нет пропущенных узлов/линий / `rs_out` паритет» — **выполнен** на 2 и 1.

## Корневая причина (закрыта)

`LINE_PRIORITY` без `regulator_press` → 509 reject → пустой `pressregulators` →
пустой `rs_out`. Исправлено + reconvert. См. ниже и
[10-orphan-pressregulators.md](10-orphan-pressregulators.md) про 16 сирот.

## Фрагмент 2 (после фикса)

| БД | calc | Tn |
|---|---|---|
| almatygid | 2 | −25 |
| tgid_gis | 5 | −25 |

| таблица | A / B | вердикт |
|---|---|---|
| `rs_out` | 2 / 2 | **совпадает** (13 полей) |
| `ns_out` | 2 / 2 | **совпадает** |
| `us_out` | 4908 / 4908 | positive id sets равны; Δ `pih` ≤ 48.5 |
| `ut_out` | 2773 / 2773 | max \|Δ\| `a13` **496.9** ≈ шум одной БД |
| `pt_out` | 1304 / 1304 | 5 ячеек, макс 5.16 |

JSON: [calc_regression_report.json](schema/calc_regression_report.json).

## Фрагмент 1 (доп. прогон)

Магистраль (~2028 узлов, 72 PR в обеих БД).

| БД | calc | Tn |
|---|---|---|
| almatygid | 3 | −25 |
| tgid_gis | 6 | −25 |

| таблица | A / B | вердикт |
|---|---|---|
| `rs_out` | **61 / 61** | **совпадает** (13 полей) |
| `ns_out` | **34 / 34** | **совпадает** |
| `us_out` / `ut_out` / `pt_out` | равное число строк | positive `nodeid` sets **равны** |

Числовые выбросы: на виртуальных (отрицательных) узлах температуры уходят
в ~1e15; `ut_out.t2` один разрыв ~1e17. Это похоже на разъезд итераций на
большой схеме, не на отсутствие объектов. «Здравый» `a13` всё ещё шумит
сильнее, чем на фрагменте 2 (сотни ячеек >100) — отдельный разбор сходимости,
не structural.

JSON: [calc_regression_f1.json](schema/calc_regression_f1.json),
[calc_regression_f1_summary.json](schema/calc_regression_f1_summary.json).

## Фрагмент 80

В `nodes` есть ~7380 узлов с `fileid=80`, но в таблице **`fragments` id=80 нет**
(то же для 91 и др. копий). `w.py` проверяет:

```sql
SELECT id FROM fragments WHERE id IN (...)
```

и при пустом ответе печатает список фрагментов + «Какая-то ошибка!!!!».
Это **данные источника**, не миграция. Рабочие id для sety: те, что есть
в `fragments` (1, 2, 5, 74, …).

## 16 pressregulators вне net

Все с `nodeid1`/`nodeid2` NULL → `line_orphan`. Не восстанавливаются.
Подробно: [10-orphan-pressregulators.md](10-orphan-pressregulators.md).

## Повтор

```powershell
.\tools\run_calc.ps1 -Compare -Fragment 2 -Tn -25 `
  -Report docs\schema\calc_regression_report.json
.\tools\run_calc.ps1 -Compare -Fragment 1 -Tn -25 `
  -Report docs\schema\calc_regression_f1.json
# не использовать fileid без строки в fragments (80, 91, …)
```
