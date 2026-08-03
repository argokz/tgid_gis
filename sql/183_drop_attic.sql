-- Удаление чердака: 161 таблица из 167.
--
-- Решение заказчика от 3 августа 2026. Архив при этом не теряется:
-- исходная база almatygid жива на том же сервере и содержит те же
-- таблицы. Из 167 таблиц attic в almatygid отсутствуют только 23, и все
-- 23 — служебные *_legacy, созданные самим переносом (переименованные
-- оригиналы, чьи базовые имена в almatygid есть).
--
-- Шесть таблиц ОСТАЮТСЯ: их читают представления compat, а те нужны
-- экспорту фрагмента в gid8 (docs/18, шаг 65 и sql/177):
--   magistrali, internalnodes, pavilions,
--   overgroundnodes, undergroundnodes, uninstallednodes
-- Когда экспорт перестанет их требовать, чердак можно закрыть целиком.
--
-- Перед применением сделан полный дамп:
--   H:\backup\tgid_gis_pered_udaleniem_attic_20260803_2258.dump (105 МБ)
-- Оглавление дампа проверено: 5786 записей, 334 записи по attic.
--
-- CASCADE стоит осознанно: у таблиц чердака остались собственные
-- индексы и ограничения, но зависимостей ИЗВНЕ у них нет — это
-- проверено запросом по pg_depend, снаружи на attic ссылаются ровно те
-- шесть представлений compat, которые здесь исключены.

BEGIN;

DROP TABLE IF EXISTS attic.act CASCADE;
DROP TABLE IF EXISTS attic.administrativedistricts CASCADE;
DROP TABLE IF EXISTS attic.airheaters_legacy CASCADE;
DROP TABLE IF EXISTS attic.bridge_crossing CASCADE;
DROP TABLE IF EXISTS attic.brigades CASCADE;
DROP TABLE IF EXISTS attic.building_consumer_mixed_legacy CASCADE;
DROP TABLE IF EXISTS attic.building_tu_mixed_legacy CASCADE;
DROP TABLE IF EXISTS attic.bypass CASCADE;
DROP TABLE IF EXISTS attic.calculatedloads CASCADE;
DROP TABLE IF EXISTS attic.calculatedloadsfact CASCADE;
DROP TABLE IF EXISTS attic.calculations CASCADE;
DROP TABLE IF EXISTS attic.capital2 CASCADE;
DROP TABLE IF EXISTS attic.capital2_dokumenty CASCADE;
DROP TABLE IF EXISTS attic.capital2_meropriyatiya CASCADE;
DROP TABLE IF EXISTS attic.channelcoverconstructions CASCADE;
DROP TABLE IF EXISTS attic.channels CASCADE;
DROP TABLE IF EXISTS attic.channeltypes CASCADE;
DROP TABLE IF EXISTS attic.connectnodes_legacy CASCADE;
DROP TABLE IF EXISTS attic.consumptregulators CASCADE;
DROP TABLE IF EXISTS attic.dampers_legacy CASCADE;
DROP TABLE IF EXISTS attic.defectdocuments CASCADE;
DROP TABLE IF EXISTS attic.defectopis CASCADE;
DROP TABLE IF EXISTS attic.defekt CASCADE;
DROP TABLE IF EXISTS attic.defekt2 CASCADE;
DROP TABLE IF EXISTS attic.defekt2_dokumenty CASCADE;
DROP TABLE IF EXISTS attic.defekt2_meropriyatiya CASCADE;
DROP TABLE IF EXISTS attic.deployedtempgraphsfact CASCADE;
DROP TABLE IF EXISTS attic.dgu CASCADE;
DROP TABLE IF EXISTS attic.dgudocuments CASCADE;
DROP TABLE IF EXISTS attic.diaphragms_legacy CASCADE;
DROP TABLE IF EXISTS attic.districts CASCADE;
DROP TABLE IF EXISTS attic.docdiagosv CASCADE;
DROP TABLE IF EXISTS attic.docdiagvskr CASCADE;
DROP TABLE IF EXISTS attic.docdiagvyr CASCADE;
DROP TABLE IF EXISTS attic.dolzhnosti_es CASCADE;
DROP TABLE IF EXISTS attic.dolzhnosti_korrozia CASCADE;
DROP TABLE IF EXISTS attic.drenazhnyy_kran CASCADE;
DROP TABLE IF EXISTS attic.drenazhnyy_truboprovod CASCADE;
DROP TABLE IF EXISTS attic.duker CASCADE;
DROP TABLE IF EXISTS attic.edv CASCADE;
DROP TABLE IF EXISTS attic.edvdocuments CASCADE;
DROP TABLE IF EXISTS attic.electrodocuments CASCADE;
DROP TABLE IF EXISTS attic.electrodocumentsist CASCADE;
DROP TABLE IF EXISTS attic.electrodocumentspr CASCADE;
DROP TABLE IF EXISTS attic.elevators_legacy CASCADE;
DROP TABLE IF EXISTS attic.fragments CASCADE;
DROP TABLE IF EXISTS attic.generalizedconsumers_legacy CASCADE;
DROP TABLE IF EXISTS attic.gilza_es CASCADE;
DROP TABLE IF EXISTS attic.gilza_esdocuments CASCADE;
DROP TABLE IF EXISTS attic.granitsy_vertikalnyh_kompensatorov CASCADE;
DROP TABLE IF EXISTS attic.gruzob CASCADE;
DROP TABLE IF EXISTS attic.gruzobdocuments CASCADE;
DROP TABLE IF EXISTS attic.heatchambers_legacy CASCADE;
DROP TABLE IF EXISTS attic.heatexchangers_legacy CASCADE;
DROP TABLE IF EXISTS attic.heatlosessourcelist CASCADE;
DROP TABLE IF EXISTS attic.heatpipesections_legacy CASCADE;
DROP TABLE IF EXISTS attic.heatpipesectionsharnessfact CASCADE;
DROP TABLE IF EXISTS attic.heatsources_legacy CASCADE;
DROP TABLE IF EXISTS attic.heatsysobjmaterials CASCADE;
DROP TABLE IF EXISTS attic.ispravl CASCADE;
DROP TABLE IF EXISTS attic.istochnik_elektrosnabzheniya CASCADE;
DROP TABLE IF EXISTS attic.istochniki_teplosnabzheniya CASCADE;
DROP TABLE IF EXISTS attic.kabelnyy_kanal_es CASCADE;
DROP TABLE IF EXISTS attic.kabelnyy_kanal_esdocuments CASCADE;
DROP TABLE IF EXISTS attic.kamera_opuska_ili_podema CASCADE;
DROP TABLE IF EXISTS attic.kanal CASCADE;
DROP TABLE IF EXISTS attic.kapremont_elementy CASCADE;
DROP TABLE IF EXISTS attic.kapremont_kolodtsy_opory_nadzemnyy CASCADE;
DROP TABLE IF EXISTS attic.kapremont_kolodtsy_opory_nadzemnyy2 CASCADE;
DROP TABLE IF EXISTS attic.kapremont_kolodtsy_opory_podzemnyy CASCADE;
DROP TABLE IF EXISTS attic.kapremont_kolodtsy_opory_podzemnyy2 CASCADE;
DROP TABLE IF EXISTS attic.kapremont_meropriyatiya CASCADE;
DROP TABLE IF EXISTS attic.kapremont_planiruemye_meropriyatiya_trubopr_izolyatsiya_oborudo CASCADE;
DROP TABLE IF EXISTS attic.kapremont_trubopr_izol CASCADE;
DROP TABLE IF EXISTS attic.kolodtsy CASCADE;
DROP TABLE IF EXISTS attic.kompensator CASCADE;
DROP TABLE IF EXISTS attic.kontrol_tehnicheskogo_sostoyaniya CASCADE;
DROP TABLE IF EXISTS attic.kontrol_tehnicheskogo_sostoyaniya_dokumenty CASCADE;
DROP TABLE IF EXISTS attic.linesobj_legacy CASCADE;
DROP TABLE IF EXISTS attic.liniya_elektroperedach CASCADE;
DROP TABLE IF EXISTS attic.link_elist_diz CASCADE;
DROP TABLE IF EXISTS attic.link_elist_grpod CASCADE;
DROP TABLE IF EXISTS attic.link_elist_trans CASCADE;
DROP TABLE IF EXISTS attic.localhydroresistances2_legacy CASCADE;
DROP TABLE IF EXISTS attic.lyuki CASCADE;
DROP TABLE IF EXISTS attic.materialcharacteristics CASCADE;
DROP TABLE IF EXISTS attic.materialcharacteristicsfact CASCADE;
DROP TABLE IF EXISTS attic.mufta CASCADE;
DROP TABLE IF EXISTS attic.muftadocuments CASCADE;
DROP TABLE IF EXISTS attic.nagruzki_po_elevatoram CASCADE;
DROP TABLE IF EXISTS attic.nasosnye_stantsii CASCADE;
DROP TABLE IF EXISTS attic.nodes_legacy CASCADE;
DROP TABLE IF EXISTS attic.opora CASCADE;
DROP TABLE IF EXISTS attic.opora_es CASCADE;
DROP TABLE IF EXISTS attic.opora_esdocuments CASCADE;
DROP TABLE IF EXISTS attic.opora_skolzyaschaya CASCADE;
DROP TABLE IF EXISTS attic.opresacts CASCADE;
DROP TABLE IF EXISTS attic.opresdocuments CASCADE;
DROP TABLE IF EXISTS attic.opresmeropr CASCADE;
DROP TABLE IF EXISTS attic.opressovka_otvetstvennye CASCADE;
DROP TABLE IF EXISTS attic.opressovka_sektsioniruyuschaya_zadvizhka CASCADE;
DROP TABLE IF EXISTS attic.opressovka_uchastki_remonta CASCADE;
DROP TABLE IF EXISTS attic.osmotrdocuments CASCADE;
DROP TABLE IF EXISTS attic.owners CASCADE;
DROP TABLE IF EXISTS attic.pavilion CASCADE;
DROP TABLE IF EXISTS attic.perehlesty_setey CASCADE;
DROP TABLE IF EXISTS attic.perehod_diametra CASCADE;
DROP TABLE IF EXISTS attic.peremychki CASCADE;
DROP TABLE IF EXISTS attic.pipe_reconstructed_mixed_legacy CASCADE;
DROP TABLE IF EXISTS attic.pipesections CASCADE;
DROP TABLE IF EXISTS attic.plan_remont CASCADE;
DROP TABLE IF EXISTS attic.podpis CASCADE;
DROP TABLE IF EXISTS attic.pressdropregulators CASCADE;
DROP TABLE IF EXISTS attic.pressregulators_legacy CASCADE;
DROP TABLE IF EXISTS attic.pribor CASCADE;
DROP TABLE IF EXISTS attic.priemnik_elektrosnabzheniya CASCADE;
DROP TABLE IF EXISTS attic.pumps_legacy CASCADE;
DROP TABLE IF EXISTS attic.pumpstations_legacy CASCADE;
DROP TABLE IF EXISTS attic.realconsumerdocuments1 CASCADE;
DROP TABLE IF EXISTS attic.realconsumerdocuments2 CASCADE;
DROP TABLE IF EXISTS attic.realconsumerdocuments3 CASCADE;
DROP TABLE IF EXISTS attic.realconsumers_legacy CASCADE;
DROP TABLE IF EXISTS attic.refillnodes_legacy CASCADE;
DROP TABLE IF EXISTS attic.regularmatures CASCADE;
DROP TABLE IF EXISTS attic.regulatortypes CASCADE;
DROP TABLE IF EXISTS attic.remontdocuments CASCADE;
DROP TABLE IF EXISTS attic.reversevalves CASCADE;
DROP TABLE IF EXISTS attic.setpressnodes CASCADE;
DROP TABLE IF EXISTS attic.shield CASCADE;
DROP TABLE IF EXISTS attic.shurf2 CASCADE;
DROP TABLE IF EXISTS attic.shurf2_dokumenty CASCADE;
DROP TABLE IF EXISTS attic.shurf2_meropriyatiya CASCADE;
DROP TABLE IF EXISTS attic.shurfdocuments CASCADE;
DROP TABLE IF EXISTS attic.spisok_kamer_drenazh CASCADE;
DROP TABLE IF EXISTS attic.standartequipment CASCADE;
DROP TABLE IF EXISTS attic.sysdiagrams CASCADE;
DROP TABLE IF EXISTS attic.systemradiators_legacy CASCADE;
DROP TABLE IF EXISTS attic.tab CASCADE;
DROP TABLE IF EXISTS attic.temp_line CASCADE;
DROP TABLE IF EXISTS attic.temp_node CASCADE;
DROP TABLE IF EXISTS attic.threewayvalves_legacy CASCADE;
DROP TABLE IF EXISTS attic.tip_kamery CASCADE;
DROP TABLE IF EXISTS attic.tip_otklyucheniya_zapornoy_armatury CASCADE;
DROP TABLE IF EXISTS attic.tip_perekrytiya_kamery CASCADE;
DROP TABLE IF EXISTS attic.tkamera CASCADE;
DROP TABLE IF EXISTS attic.tkameradocuments CASCADE;
DROP TABLE IF EXISTS attic.towns CASCADE;
DROP TABLE IF EXISTS attic.transf CASCADE;
DROP TABLE IF EXISTS attic.transfdocuments CASCADE;
DROP TABLE IF EXISTS attic.truby_rekonstruiruemye CASCADE;
DROP TABLE IF EXISTS attic.tstrp CASCADE;
DROP TABLE IF EXISTS attic.tubes CASCADE;
DROP TABLE IF EXISTS attic.ugol_povorota_truboprovoda CASCADE;
DROP TABLE IF EXISTS attic.ulitsy_almaty CASCADE;
DROP TABLE IF EXISTS attic.uzel_kanala CASCADE;
DROP TABLE IF EXISTS attic.vertikalnyy_podem_opusk_truprovoda CASCADE;
DROP TABLE IF EXISTS attic.visualmetriccontrol CASCADE;
DROP TABLE IF EXISTS attic.vladeltsy_es CASCADE;
DROP TABLE IF EXISTS attic.vozdushnik CASCADE;
DROP TABLE IF EXISTS attic.vyd CASCADE;
DROP TABLE IF EXISTS attic.wdodevices CASCADE;

COMMIT;

SELECT count(*) AS ostalos_v_attic
FROM pg_class c JOIN pg_namespace n ON n.oid=c.relnamespace
WHERE n.nspname='attic' AND c.relkind='r';
