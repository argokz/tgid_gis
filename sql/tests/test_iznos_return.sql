\set ON_ERROR_STOP on

DO $$
BEGIN
  ASSERT to_regclass('ops.calculation_iznos') IS NOT NULL,
    'ops.calculation_iznos должна существовать';
  ASSERT to_regclass('ops.iznos') IS NOT NULL,
    'ops.iznos должна существовать';
  ASSERT to_regclass('attic.calculation_iznos') IS NULL,
    'attic.calculation_iznos должна быть перенесена';
  ASSERT to_regclass('attic.iznos') IS NULL,
    'attic.iznos должна быть перенесена';
END $$;

SELECT 'test_iznos_return OK' AS result;
