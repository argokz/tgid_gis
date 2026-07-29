-- Откат подстановки таблиц-подтипов.
\set ON_ERROR_STOP on

BEGIN;

DROP VIEW IF EXISTS public.connectnodes;
ALTER TABLE public.connectnodes_legacy RENAME TO connectnodes;
DROP VIEW IF EXISTS public.generalizedconsumers;
ALTER TABLE public.generalizedconsumers_legacy RENAME TO generalizedconsumers;
DROP VIEW IF EXISTS public.heatchambers;
ALTER TABLE public.heatchambers_legacy RENAME TO heatchambers;
DROP VIEW IF EXISTS public.heatsources;
ALTER TABLE public.heatsources_legacy RENAME TO heatsources;
DROP VIEW IF EXISTS public.pumpstations;
ALTER TABLE public.pumpstations_legacy RENAME TO pumpstations;
DROP VIEW IF EXISTS public.realconsumers;
ALTER TABLE public.realconsumers_legacy RENAME TO realconsumers;
DROP VIEW IF EXISTS public.refillnodes;
ALTER TABLE public.refillnodes_legacy RENAME TO refillnodes;
DROP VIEW IF EXISTS public.threewayvalves;
ALTER TABLE public.threewayvalves_legacy RENAME TO threewayvalves;
DROP VIEW IF EXISTS public.airheaters;
ALTER TABLE public.airheaters_legacy RENAME TO airheaters;
DROP VIEW IF EXISTS public.dampers;
ALTER TABLE public.dampers_legacy RENAME TO dampers;
DROP VIEW IF EXISTS public.diaphragms;
ALTER TABLE public.diaphragms_legacy RENAME TO diaphragms;
DROP VIEW IF EXISTS public.elevators;
ALTER TABLE public.elevators_legacy RENAME TO elevators;
DROP VIEW IF EXISTS public.heatexchangers;
ALTER TABLE public.heatexchangers_legacy RENAME TO heatexchangers;
DROP VIEW IF EXISTS public.heatpipesections;
ALTER TABLE public.heatpipesections_legacy RENAME TO heatpipesections;
DROP VIEW IF EXISTS public.localhydroresistances2;
ALTER TABLE public.localhydroresistances2_legacy RENAME TO localhydroresistances2;
DROP VIEW IF EXISTS public.pressregulators;
ALTER TABLE public.pressregulators_legacy RENAME TO pressregulators;
DROP VIEW IF EXISTS public.pumps;
ALTER TABLE public.pumps_legacy RENAME TO pumps;
DROP VIEW IF EXISTS public.systemradiators;
ALTER TABLE public.systemradiators_legacy RENAME TO systemradiators;

COMMIT;

\echo 'Таблицы-подтипов возвращены'
