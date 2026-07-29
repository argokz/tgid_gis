-- СГЕНЕРИРОВАНО tools/gen_subtype_views.py
-- Подстановка представлений вместо таблиц-подтипов.
-- Оригиналы сохраняются под именами *_legacy.
\set ON_ERROR_STOP on

BEGIN;

ALTER TABLE public.connectnodes RENAME TO connectnodes_legacy;
CREATE VIEW public.connectnodes AS SELECT * FROM net.v_connectnodes;
ALTER TABLE public.generalizedconsumers RENAME TO generalizedconsumers_legacy;
CREATE VIEW public.generalizedconsumers AS SELECT * FROM net.v_generalizedconsumers;
ALTER TABLE public.heatchambers RENAME TO heatchambers_legacy;
CREATE VIEW public.heatchambers AS SELECT * FROM net.v_heatchambers;
ALTER TABLE public.heatsources RENAME TO heatsources_legacy;
CREATE VIEW public.heatsources AS SELECT * FROM net.v_heatsources;
ALTER TABLE public.pumpstations RENAME TO pumpstations_legacy;
CREATE VIEW public.pumpstations AS SELECT * FROM net.v_pumpstations;
ALTER TABLE public.realconsumers RENAME TO realconsumers_legacy;
CREATE VIEW public.realconsumers AS SELECT * FROM net.v_realconsumers;
ALTER TABLE public.refillnodes RENAME TO refillnodes_legacy;
CREATE VIEW public.refillnodes AS SELECT * FROM net.v_refillnodes;
ALTER TABLE public.threewayvalves RENAME TO threewayvalves_legacy;
CREATE VIEW public.threewayvalves AS SELECT * FROM net.v_threewayvalves;
ALTER TABLE public.airheaters RENAME TO airheaters_legacy;
CREATE VIEW public.airheaters AS SELECT * FROM net.v_airheaters;
ALTER TABLE public.dampers RENAME TO dampers_legacy;
CREATE VIEW public.dampers AS SELECT * FROM net.v_dampers;
ALTER TABLE public.diaphragms RENAME TO diaphragms_legacy;
CREATE VIEW public.diaphragms AS SELECT * FROM net.v_diaphragms;
ALTER TABLE public.elevators RENAME TO elevators_legacy;
CREATE VIEW public.elevators AS SELECT * FROM net.v_elevators;
ALTER TABLE public.heatexchangers RENAME TO heatexchangers_legacy;
CREATE VIEW public.heatexchangers AS SELECT * FROM net.v_heatexchangers;
ALTER TABLE public.heatpipesections RENAME TO heatpipesections_legacy;
CREATE VIEW public.heatpipesections AS SELECT * FROM net.v_heatpipesections;
ALTER TABLE public.localhydroresistances2 RENAME TO localhydroresistances2_legacy;
CREATE VIEW public.localhydroresistances2 AS SELECT * FROM net.v_localhydroresistances2;
ALTER TABLE public.pressregulators RENAME TO pressregulators_legacy;
CREATE VIEW public.pressregulators AS SELECT * FROM net.v_pressregulators;
ALTER TABLE public.pumps RENAME TO pumps_legacy;
CREATE VIEW public.pumps AS SELECT * FROM net.v_pumps;
ALTER TABLE public.systemradiators RENAME TO systemradiators_legacy;
CREATE VIEW public.systemradiators AS SELECT * FROM net.v_systemradiators;

COMMIT;

\echo 'Таблицы-подтипов заменены представлениями над net'
