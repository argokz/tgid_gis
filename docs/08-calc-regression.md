# Регрессия расчёта sety: almatygid vs tgid_gis

## Инструменты

| Файл | Назначение |
|---|---|
| [tools/run_calc.ps1](../tools/run_calc.ps1) | Запуск `ww.py` (sety) |
| [tools/compare_calc.py](../tools/compare_calc.py) | Сверка `*_OUT` двух БД |
| [tools/compare_calc_ids.py](../tools/compare_calc_ids.py) | Сверка двух `calculation.id` |

`Tn` по умолчанию **−25**. Фрагмент должен существовать в таблице `fragments`
(fileid 80/91 и др. копии — **нет**, sety падает сразу).

## Итог

Подробности: **[09-calc-regression-results.md](09-calc-regression-results.md)**.

- Фр. **2** и **1**: структура совпала (`rs_out`, наборы узлов).
- Фр. **80**: нет в `fragments` — не баг миграции.
- 16 PR без концов: [10-orphan-pressregulators.md](10-orphan-pressregulators.md).

Баг `LINE_PRIORITY` / `regulator_press` закрыт.

## Команды

```powershell
.\tools\run_calc.ps1 -Compare -Fragment 2 -Tn -25 `
  -Report docs\schema\calc_regression_report.json
.\tools\run_calc.ps1 -Compare -Fragment 1 -Tn -25 `
  -Report docs\schema\calc_regression_f1.json
```
