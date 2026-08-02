# -*- coding: utf-8 -*-
"""Генератор function-matrix.canvas.tsx из legacy_inventory.json + статусная модель."""
import json
import io
import sys
import os

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WS = os.path.dirname(ROOT)

BLOCKS = {
    "network_edit": "Редактирование сети",
    "map_navigation": "Навигация по карте",
    "calculations": "Расчёты",
    "queries_reports": "Запросы и отчёты",
    "technical_conditions": "Техусловия (ТУ)",
    "maintenance": "Ремонты и дефекты",
    "corrosion": "Коррозия",
    "passport": "Паспортизация",
    "heat_losses": "Теплопотери",
    "gis_exchange": "Обмен / импорт-экспорт",
    "administration": "Администрирование",
    "reference_data": "Справочники",
    "other": "Прочее (карта, вид, настройки)",
}
BLOCK_ORDER = list(BLOCKS.keys())

DONE = {
    "aProtNew": "Семейство протяжённости, шаг 17 (A/B с almatygid)",
    "aProtMag": "Семейство протяжённости, шаг 17",
    "aProtRs": "Семейство протяжённости, шаг 17",
    "aProtKot": "Семейство протяжённости, шаг 17",
    "aProtPass": "Семейство протяжённости, шаг 17",
    "aZap1": "Объём сети, шаг 21",
    "aZap3": "Теплопотребление, шаг 22",
    "aZap4": "Теплопотребление (закрытые), шаг 22",
    "aZap5": "Теплопотребление (открытые), шаг 22",
    "aZap6": "Закрытые потребители, шаг 23",
    "aPotNagr0": "Нулевые нагрузки, шаг 24",
    "aPotrOtkl": "Отключённые потребители, шаг 25",
    "aUtZakr": "Закрытые участки, шаг 26",
    "aUtZakrAll": "Отключённые участки, шаг 27",
    "aMapSearch": "Поиск объектов, шаг 16",
    "aDoIt": "Фактический расчёт, шаги 67–71 (фр. 1, 2, 72, 76, 91)",
    "aFindKti": "Перенесён из gid6, шаг 72 (A/B: 5979≡5982 − 3 orphan)",
    "aFindTuIst": "Перенесён из gid6, шаг 72 (A/B: 72 источника 1:1)",
    "aColorOnlyPts": "Перенесён из gid6, шаг 72 (флаг отрисовки ПТС)",
    "aHelpFinder": "Шаг 72: открытие help/index.html|chm|pdf рядом с exe",
    "aZoom": "Режим рамки-масштаба через regimGroup (работал), шаг 72",
    "aFon": "Чистый UI, БД не использует; шаг 72",
    "aFont": "Чистый UI, БД не использует; шаг 72",
    "aFont2": "Чистый UI, БД не использует; шаг 72",
    "aFontMag": "Чистый UI, БД не использует; шаг 72",
    "aFontPanel": "Чистый UI, БД не использует; шаг 72",
    "aMasPodpis": "Чистый UI, БД не использует; шаг 72",
    "aMasall": "Чистый UI, БД не использует; шаг 72",
    "aMasshtab": "Чистый UI, БД не использует; шаг 72",
    "aNavigator": "Чистый UI, БД не использует; шаг 72",
    "aFindcoordDeg": "Чистый UI (координаты), БД не использует; шаг 72",
    "aClearOut": "Шаг 73: перенесён из gid6 (DELETE старых calculation)",
    "aTableDan": "Шаг 74: редактор таблиц (information_schema → DbWindow)",
    "aTuTableNeiz": "Шаг 74: ТУ без привязки к зданиям (A/B 177≡177)",
    "aTuIst": "Шаг 74: отчёт по источникам (tu/1.sql, A/B 1:1)",
    "aTuExcel": "Шаг 74: print_tu_itog — год из диалога, ГВС/coef, колонка Пар",
    "aCreateSortNode": "Шаг 75: порт gid6 sortNode/sortLine → ops.sortnodesforuchastok/sortlinesforuchastok",
    "aInfoGid": "Шаг 73: info_gid(pt) реализован — диспетчер узел/линия → PropertyDial",
    "aRasList": "Шаг 73: список расчётов (DbWindow, calc.calculation), A/B OK",
    "aExport": "Шаг 73: новая реализация (в gid6 мёртвая) — us_out/ut_out в TXT",
    "aRasprMag": "Шаг 73: A/B 24≡24, работает через compat/ref",
    "aSetKodRs": "Шаг 73: общий путь записи setSomething (проверен шагами 50/69)",
    "aSetOpenGvsT": "Шаг 73: общий путь записи setSomething (проверен шагами 50/69)",
    "aSetTr": "Шаг 73: общий путь записи setSomething (проверен шагами 50/69)",
    "aPjezo": "Шаг 73: пьезометр реализован (Pjezo.cpp)",
    "aListPjezo": "Шаг 73: список направлений реализован (MMenuDial)",
    "aSavePjezo": "Шаг 73: insertIntoDatabase directions/deployedDirections (ref)",
}

ENGINE = {
    "aDoItDr": "Плановый режим того же движка sety",
    "aDoItList": "Тот же движок по списку фрагментов",
    "aDoItListDr": "Тот же движок по списку, плановый",
    "aInfoGid": "Результаты лежат в calc.*; окно не проверено",
    "aRasList": "Список расчётов = calc.calculation; окно не проверено",
    "aPjezo": "Пьезометр по calc.*; окно не проверено",
    "aListPjezo": "Список направлений пьезометра",
    "aSavePjezo": "Сохранение направления пьезометра",
}

EDITOR_EXTRA = {
    "aInfo": "Карточки объектов, шаги 65–66",
    "aEditUndo": "Отмена в редакторе, шаг 66",
    "aC3Add": "Внутренние схемы (b5.cpp), шаги 63–68",
    "aExportFragment": "Экспорт фрагмента, шаг 64",
    "aExportFragmentVyd": "Экспорт выделенного фрагмента, шаг 64",
    "aExportFragmentsMulty": "Экспорт всех фрагментов, шаг 64",
}

TABLES_NOTE = "Таблицы в tgid_gis готовы (шаг 57); UI не проверен"

CORROSION = {
    "aKorrozAdd", "aKorrozDel", "aKorrozInfo", "aKorrozTable",
    "aKorroziaCurrent", "aKorroziaOnoff", "aKorroziaSezon", "aSetKorrozia",
    "aKorrozDoc1", "aKorrozDoc2", "aKorrozDoc3", "aKorroziaUpdate",
    "aKorroziaZhurnal1", "aKorroziaZhurnal2", "aKorroziaZhurnal3",
    "aFragmentPolyKorroziaAr", "aViewToolbarKorrozia",
}

MAINTENANCE_TABLES = {
    "aListOpres", "aListRemont2", "aBottomRemont", "aEditor2",
    "aRemontPlan", "aSetDate3", "aSetPipeRemontType", "aIznos",
}

SPECIAL_NOTES = {
    "aQuery": "Частично закрыт поиском объектов (шаг 16); произвольный SQL — нет",
    "aElectroAdd": "Электрические объекты: данных нет в tgid_gis, шаг 68",
    "aElectroInfo": "Электрические объекты: данных нет в tgid_gis, шаг 68",
    "aElectroDel": "Электрические объекты: данных нет в tgid_gis, шаг 68",
    "aElectroTable": "Электрические объекты: данных нет в tgid_gis, шаг 68",
    "aZhurnalElectro": "Электрические объекты: данных нет в tgid_gis, шаг 68",
    "aViewToolbarElectro": "Электрические объекты: данных нет в tgid_gis, шаг 68",
    "aAddFragment": "Объединение фрагментов, шаг 64",
    "aSqlCopyFragment": "Вставка фрагмента, шаг 64",
    "aSqlDelFragment": "Удаление фрагмента, шаг 64",
    "aImportFragmentMdb": "Конвертор старых форматов ТГИД-05/07",
    "aZn": "Узел с заданным напором — критично для расчётов",
}


LABEL_FIX = {
    "aProtKot": "Протяженность котельных",
    "aSqlSave": "Создание копии Базы данных",
}


def fix_label(s, name=""):
    if name in LABEL_FIX:
        return LABEL_FIX[name]
    if not s:
        return ""
    s = s.split("\\t")[0].split("\t")[0].strip()
    if "Р" in s and ("џ" in s or "ќ" in s or "ћ" in s or "Ђ" in s):
        try:
            s = s.encode("cp1251").decode("utf-8", errors="ignore")
        except Exception:
            pass
    return s


def is_electro(name):
    return "Electro" in name or name == "aZhurnalElectro"


def status_of(a):
    n = a["name"]
    if n in DONE:
        return "done", DONE[n]
    if n in ENGINE:
        return "engine", ENGINE[n]
    if n in EDITOR_EXTRA:
        return "editor", EDITOR_EXTRA[n]
    if n.startswith("aViewApplook"):
        return "obsolete", "Темы оформления Qt — к БД не относятся"
    if is_electro(n):
        return "obsolete", SPECIAL_NOTES.get(n, "Электрические объекты: данных нет, шаг 68")
    if n in CORROSION or n in MAINTENANCE_TABLES:
        return "tables", TABLES_NOTE
    if not a["initialized"]:
        return "dead", "Объявлена, но не инициализирована в legacy UI"
    if a["category"] == "network_edit":
        return "editor", SPECIAL_NOTES.get(n, "Контур редактора сети, шаги 63–66")
    return "todo", SPECIAL_NOTES.get(n, "")


def main():
    d = json.load(open(os.path.join(ROOT, "docs/schema/legacy_inventory.json"), encoding="utf-8"))
    rows = []
    for a in d["actions"]:
        st, note = status_of(a)
        rows.append({
            "n": a["name"],
            "l": fix_label(a.get("label") or a.get("comment") or "", a["name"]),
            "b": a["category"],
            "s": st,
            "t": note,
        })
    order = {b: i for i, b in enumerate(BLOCK_ORDER)}
    rows.sort(key=lambda r: (order.get(r["b"], 99), r["n"]))

    data = json.dumps({"blocks": BLOCKS, "order": BLOCK_ORDER, "rows": rows}, ensure_ascii=False)

    tsx = TEMPLATE.replace("__DATA__", data)
    out = os.path.join(WS, "function-matrix.canvas.tsx")
    with open(out, "w", encoding="utf-8") as f:
        f.write(tsx)
    print(f"written: {out}, rows={len(rows)}")
    from collections import Counter
    print(Counter(r["s"] for r in rows))


TEMPLATE = r'''import { useMemo, useState } from "react";
import {
  Callout,
  Code,
  Divider,
  Grid,
  H1,
  H2,
  Pill,
  Row,
  Spacer,
  Stack,
  Stat,
  Table,
  Text,
  UsageBar,
  useCanvasState,
  useHostTheme,
} from "cursor/canvas";

type CmdRow = { n: string; l: string; b: string; s: string; t: string };
type Data = { blocks: Record<string, string>; order: string[]; rows: CmdRow[] };

const DATA: Data = __DATA__;

const STATUS: Record<string, { label: string; tone: "success" | "warning" | "danger" | "neutral"; color: "green" | "yellow" | "red" | "gray" }> = {
  done: { label: "Перенесено, проверено", tone: "success", color: "green" },
  editor: { label: "Редактор сети (шаги 63–66)", tone: "success", color: "green" },
  engine: { label: "Ядро готово, UI не проверен", tone: "warning", color: "yellow" },
  tables: { label: "Таблицы готовы, UI не начат", tone: "warning", color: "yellow" },
  todo: { label: "Не начато", tone: "danger", color: "red" },
  dead: { label: "Мёртвая кнопка в legacy UI", tone: "neutral", color: "gray" },
  obsolete: { label: "Кандидат на исключение", tone: "neutral", color: "gray" },
};

const DECISIONS: Record<string, string> = {
  req: "Обязательная",
  opt: "Опциональная",
  skip: "Не нужна",
};

function blockStats(rows: CmdRow[]) {
  const g = { good: 0, mid: 0, bad: 0, gray: 0 };
  for (const r of rows) {
    if (r.s === "done" || r.s === "editor") g.good++;
    else if (r.s === "engine" || r.s === "tables") g.mid++;
    else if (r.s === "todo") g.bad++;
    else g.gray++;
  }
  return g;
}

export default function FunctionMatrix() {
  const theme = useHostTheme();
  const [block, setBlock] = useState<string>("all");
  const [status, setStatus] = useState<string>("all");
  const [onlyUndecided, setOnlyUndecided] = useState(false);
  const [decisions, setDecisions] = useCanvasState<Record<string, string>>("decisions", {});

  const rows = DATA.rows;
  const decided = Object.keys(decisions).length;
  const decCounts = useMemo(() => {
    const c = { req: 0, opt: 0, skip: 0 };
    for (const v of Object.values(decisions)) if (v in c) c[v as keyof typeof c]++;
    return c;
  }, [decisions]);

  const total = blockStats(rows);
  const filtered = rows.filter(
    (r) =>
      (block === "all" || r.b === block) &&
      (status === "all" || r.s === status) &&
      (!onlyUndecided || !decisions[r.n])
  );

  const setDecision = (name: string, val: string) =>
    setDecisions((prev) => {
      const next = { ...prev };
      if (next[name] === val) delete next[name];
      else next[name] = val;
      return next;
    });

  return (
    <Stack gap={20} style={{ padding: 20 }}>
      <Stack gap={6}>
        <H1>Функциональная матрица команд gid8</H1>
        <Text tone="secondary">
          {rows.length} команд интерфейса из инвентаризации (docs/12), сгруппированы в {DATA.order.length} блоков.
          Статус — техническая оценка миграции на tgid_gis. Решение — ваш бизнес-выбор: что обязательно переносим,
          что опционально, что исключаем. Разметка сохраняется автоматически.
        </Text>
      </Stack>

      <Grid columns={5} gap={12}>
        <Stat value={rows.length} label="Всего команд" />
        <Stat value={total.good} label="Перенесено / редактор" tone="success" />
        <Stat value={total.mid} label="Ядро или таблицы готовы" tone="warning" />
        <Stat value={total.bad} label="Не начато" tone="danger" />
        <Stat value={`${decided}`} label={`Размечено: ${decCounts.req} обяз. / ${decCounts.opt} опц. / ${decCounts.skip} не нужна`} tone="info" />
      </Grid>

      <Callout tone="info" title="Как утверждать">
        Пройдите по блокам фильтрами ниже и отметьте каждую команду: «Обяз.», «Опц.» или «Не нужна»
        (повторный клик снимает отметку). Когда закончите — напишите в чат «зафиксируй матрицу»,
        и решения будут перенесены в docs/12-legacy-function-inventory.md.
      </Callout>

      <Divider />

      <Stack gap={10}>
        <H2>Блоки</H2>
        <Stack gap={8}>
          {DATA.order.map((b) => {
            const brows = rows.filter((r) => r.b === b);
            const g = blockStats(brows);
            return (
              <Row key={b} gap={12} align="center">
                <div style={{ width: 230 }}>
                  <Text size="small" weight={block === b ? "semibold" : "normal"}>
                    {DATA.blocks[b]} <Text size="small" tone="tertiary">({brows.length})</Text>
                  </Text>
                </div>
                <div style={{ flex: 1 }}>
                  <UsageBar
                    total={brows.length}
                    segments={[
                      { id: "good", value: g.good, color: "green" },
                      { id: "mid", value: g.mid, color: "yellow" },
                      { id: "bad", value: g.bad, color: "red" },
                      { id: "gray", value: g.gray, color: "gray" },
                    ]}
                  />
                </div>
                <Pill size="sm" active={block === b} onClick={() => setBlock(block === b ? "all" : b)}>
                  фильтр
                </Pill>
              </Row>
            );
          })}
        </Stack>
        <Row gap={16} align="center">
          <Row gap={6} align="center"><SwatchDot color={theme.category.green} /><Text size="small" tone="secondary">перенесено</Text></Row>
          <Row gap={6} align="center"><SwatchDot color={theme.category.yellow} /><Text size="small" tone="secondary">частично готово</Text></Row>
          <Row gap={6} align="center"><SwatchDot color={theme.category.red} /><Text size="small" tone="secondary">не начато</Text></Row>
          <Row gap={6} align="center"><SwatchDot color={theme.category.gray} /><Text size="small" tone="secondary">мёртвое / исключить</Text></Row>
        </Row>
      </Stack>

      <Divider />

      <Stack gap={10}>
        <Row align="center" gap={12}>
          <H2>Команды</H2>
          <Spacer />
          <Text size="small" tone="tertiary">показано {filtered.length} из {rows.length}</Text>
        </Row>

        <Row gap={6} wrap align="center">
          <Text size="small" tone="secondary">Блок:</Text>
          <Pill size="sm" active={block === "all"} onClick={() => setBlock("all")}>все</Pill>
          {DATA.order.map((b) => (
            <Pill key={b} size="sm" active={block === b} onClick={() => setBlock(b)}>
              {DATA.blocks[b]}
            </Pill>
          ))}
        </Row>

        <Row gap={6} wrap align="center">
          <Text size="small" tone="secondary">Статус:</Text>
          <Pill size="sm" active={status === "all"} onClick={() => setStatus("all")}>все</Pill>
          {Object.entries(STATUS).map(([k, v]) => (
            <Pill key={k} size="sm" active={status === k} onClick={() => setStatus(k)}>
              {v.label}
            </Pill>
          ))}
          <Divider style={{ width: 1, height: 16 }} />
          <Pill size="sm" active={onlyUndecided} onClick={() => setOnlyUndecided(!onlyUndecided)}>
            только без решения
          </Pill>
        </Row>

        <Table
          framed
          striped
          stickyHeader
          headers={["Команда", "Назначение", "Статус миграции", "Комментарий", "Решение"]}
          columnAlign={["left", "left", "left", "left", "left"]}
          rowTone={filtered.map((r) => STATUS[r.s].tone)}
          rows={filtered.map((r) => [
            <Text size="small" weight="medium"><Code>{r.n}</Code></Text>,
            <Text size="small">{r.l}</Text>,
            <Text size="small" tone={r.s === "done" || r.s === "editor" ? "primary" : r.s === "todo" ? "primary" : "secondary"}>
              {STATUS[r.s].label}
            </Text>,
            <Text size="small" tone="secondary">{r.t}</Text>,
            <Row gap={4}>
              {Object.entries(DECISIONS).map(([k, label]) => (
                <Pill key={k} size="sm" active={decisions[r.n] === k} onClick={() => setDecision(r.n, k)}>
                  {label}
                </Pill>
              ))}
            </Row>,
          ])}
          emptyMessage="Нет команд под текущие фильтры"
        />
      </Stack>
    </Stack>
  );
}

function SwatchDot({ color }: { color: string }) {
  return <div style={{ width: 10, height: 10, borderRadius: 3, background: color }} />;
}
'''


if __name__ == "__main__":
    main()
