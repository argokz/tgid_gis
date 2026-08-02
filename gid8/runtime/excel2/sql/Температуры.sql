SELECT IS3.kod_ist AS kod_ist, 1 AS M, 'январь' AS MON, 1 AS period, GT.tn_long_1 AS tn_long, GT.tn_fakt_1 AS tn_fakt, GT.tgr_long_1 AS tgr_long, GT.tgr_fakt_1 AS tgr_fakt, GO.length_1 AS length, GO.length_1_fakt AS length_fakt, 31-GO.length_1 AS lengthR, 31-GO.length_1_fakt AS lengthR_fakt, GO.t1_1 AS t1, GO.t1_1_fakt AS t1_fakt, GO.t2_1 AS t2, GO.t2_1_fakt AS t2_fakt, GO.t_1 AS t, GO.t_1_fakt AS t_fakt, Gpod_1_fakt AS Gpod_fakt, Ggvs_pribor_1_fakt AS Ggvs_pribor_fakt, Gakt_tex_1_fakt AS Gakt_tex_fakt, Gakt_avarija_1_fakt AS Gakt_avarija_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL SELECT IS3.kod_ist, 2, '‘евраль', 1, GT.tn_long_2, GT.tn_fakt_2, GT.tgr_long_2, GT.tgr_fakt_2, GO.length_2, GO.length_2_fakt, 28-GO.length_2, 28-GO.length_2_fakt, GO.t1_2, GO.t1_2_fakt, GO.t2_2, GO.t2_2_fakt, GO.t_2, GO.t_2_fakt, Gpod_2_fakt, Ggvs_pribor_2_fakt, Gakt_tex_2_fakt, Gakt_avarija_2_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL SELECT IS3.kod_ist, 3, 'ћарт', 1, GT.tn_long_3, GT.tn_fakt_3, GT.tgr_long_3, GT.tgr_fakt_3, GO.length_3, GO.length_3_fakt, 31-GO.length_3, 31-GO.length_3_fakt, GO.t1_3, GO.t1_3_fakt, GO.t2_3, GO.t2_3_fakt, GO.t_3, GO.t_3_fakt, Gpod_3_fakt, Ggvs_pribor_3_fakt, Gakt_tex_3_fakt, Gakt_avarija_3_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL 

SELECT IS3.kod_ist, 5, 'јпрель', 1, GT.tn_long_4, GT.tn_fakt_4, GT.tgr_long_4, GT.tgr_fakt_4, 

IIF(DAY(C6.end_year)<GO.length_4, DAY(C6.end_year), GO.length_4), 
IIF(DAY(C6.end_year)<GO.length_4_fakt, DAY(C6.end_year), GO.length_4_fakt), 

0, 0, GO.t1_4, GO.t1_4_fakt, GO.t2_4, GO.t2_4_fakt, 
GO.t_4, GO.t_4_fakt, 
Gpod_4_fakt, Ggvs_pribor_4_fakt, Gakt_tex_4_fakt, Gakt_avarija_4_fakt 
FROM [GO_ћес€чный график работы источника] GO, [—истема теплоснабжени€] C6, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 
WHERE IS3.kod_ist=GO.kod_ist


UNION ALL 

SELECT IS3.kod_ist, 6, 'јпрель', 0, GT.tn_long_4, GT.tn_fakt_4, GT.tgr_long_4, GT.tgr_fakt_4, 

GO.length_4-IIF(DAY(C6.end_year)<GO.length_4, DAY(C6.end_year), GO.length_4), 
GO.length_4_fakt-IIF(DAY(C6.end_year)<GO.length_4_fakt, DAY(C6.end_year), GO.length_4_fakt), 

0, 0, IS3.t1_leto, GO.t1_4_fakt, IS3.t2_leto, GO.t2_4_fakt, GO.t_4, GO.t_4_fakt, Gpod_4_fakt, Ggvs_pribor_4_fakt, Gakt_tex_4_fakt, Gakt_avarija_4_fakt FROM [GO_ћес€чный график работы источника] GO, [—истема теплоснабжени€] C6, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist

UNION ALL SELECT IS3.kod_ist, 7, 'ћай', 0, GT.tn_long_5, GT.tn_fakt_5, GT.tgr_long_5, GT.tgr_fakt_5, GO.length_5, GO.length_5_fakt, 31-GO.length_5, 31-GO.length_5_fakt, IS3.t1_leto, GO.t1_5_fakt, IS3.t2_leto, GO.t2_5_fakt, GO.t_5, GO.t_5_fakt, Gpod_5_fakt, Ggvs_pribor_5_fakt, Gakt_tex_5_fakt, Gakt_avarija_5_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL SELECT IS3.kod_ist, 8, '»юнь', 0, GT.tn_long_6, GT.tn_fakt_6, GT.tgr_long_6, GT.tgr_fakt_6, GO.length_6, GO.length_6_fakt, 30-GO.length_6, 30-GO.length_6_fakt, IS3.t1_leto, GO.t1_6_fakt, IS3.t2_leto, GO.t2_6_fakt, GO.t_6, GO.t_6_fakt, Gpod_6_fakt, Ggvs_pribor_6_fakt, Gakt_tex_6_fakt, Gakt_avarija_6_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL SELECT IS3.kod_ist, 9, '»юль', 0, GT.tn_long_7, GT.tn_fakt_7, GT.tgr_long_7, GT.tgr_fakt_7, GO.length_7, GO.length_7_fakt, 31-GO.length_7, 31-GO.length_7_fakt, IS3.t1_leto, GO.t1_7_fakt, IS3.t2_leto, GO.t2_7_fakt, GO.t_7, GO.t_7_fakt, Gpod_7_fakt, Ggvs_pribor_7_fakt, Gakt_tex_7_fakt, Gakt_avarija_7_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL SELECT IS3.kod_ist, 10, 'јвгуст', 0, GT.tn_long_8, GT.tn_fakt_8, GT.tgr_long_8, GT.tgr_fakt_8, GO.length_8, GO.length_8_fakt, 31-GO.length_8, 31-GO.length_8_fakt, IS3.t1_leto, GO.t1_8_fakt, IS3.t2_leto, GO.t2_8_fakt, GO.t_8, GO.t_8_fakt, Gpod_8_fakt, Ggvs_pribor_8_fakt, Gakt_tex_8_fakt, Gakt_avarija_8_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL SELECT IS3.kod_ist, 11, '—ент€брь',0, GT.tn_long_9, GT.tn_fakt_9, GT.tgr_long_9, GT.tgr_fakt_9, GO.length_9, GO.length_9_fakt, 30-GO.length_9, 30-GO.length_9_fakt, IS3.t1_leto, GO.t1_9_fakt, IS3.t2_leto, GO.t2_9_fakt, GO.t_9, GO.t_9_fakt, Gpod_9_fakt, Ggvs_pribor_9_fakt, Gakt_tex_9_fakt, Gakt_avarija_9_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist

UNION ALL 

SELECT IS3.kod_ist, 12, 'ќкт€брь', 0, GT.tn_long_1, GT.tn_fakt_10, GT.tgr_long_10, GT.tgr_fakt_10, 

GO.length_10-IIF(32-DAY(C6.begin_year) < GO.length_10, 32-DAY(C6.begin_year), GO.length_10), 
GO.length_10_fakt-IIF(32-DAY(C6.begin_year) < GO.length_10_fakt, 32-DAY(C6.begin_year), GO.length_10_fakt), 

0, 0, IS3.t1_leto, GO.t1_10_fakt, IS3.t2_leto, GO.t2_10_fakt, GO.t_10, GO.t_10_fakt, Gpod_10_fakt, Ggvs_pribor_10_fakt, Gakt_tex_10_fakt, Gakt_avarija_10_fakt FROM [GO_ћес€чный график работы источника] GO, [—истема теплоснабжени€] C6, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist


UNION ALL 

SELECT IS3.kod_ist, 13, 'ќкт€брь', 1, GT.tn_long_10, GT.tn_fakt_10, GT.tgr_long_10, GT.tgr_fakt_10, 

IIF(32-DAY(C6.begin_year) < GO.length_10, 32-DAY(C6.begin_year), GO.length_10), 
IIF(32-DAY(C6.begin_year) < GO.length_10_fakt, 32-DAY(C6.begin_year), GO.length_10_fakt), 

0, 0, GO.t1_10, GO.t1_10_fakt, GO.t2_10, GO.t2_10_fakt, GO.t_10, GO.t_10_fakt, Gpod_10_fakt, Ggvs_pribor_10_fakt, Gakt_tex_10_fakt, Gakt_avarija_10_fakt FROM [GO_ћес€чный график работы источника] GO, [—истема теплоснабжени€] C6, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist


UNION ALL SELECT IS3.kod_ist, 15, 'Ќо€брь', 1, GT.tn_long_11, GT.tn_fakt_11, GT.tgr_long_11, GT.tgr_fakt_11, GO.length_11, GO.length_11_fakt, 30-GO.length_11, 30-GO.length_11_fakt, GO.t1_11, GO.t1_11_fakt, GO.t2_11, GO.t2_11_fakt, GO.t_11, GO.t_11_fakt, Gpod_11_fakt, Ggvs_pribor_11_fakt, Gakt_tex_11_fakt, Gakt_avarija_11_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist
UNION ALL SELECT IS3.kod_ist, 16, 'ƒекабрь', 1, GT.tn_long_12, GT.tn_fakt_12, GT.tgr_long_12, GT.tgr_fakt_12, GO.length_12, GO.length_12_fakt, 31-GO.length_12, 31-GO.length_12_fakt, GO.t1_12, GO.t1_12_fakt, GO.t2_12, GO.t2_12_fakt, GO.t_12, GO.t_12_fakt, Gpod_12_fakt, Ggvs_pribor_12_fakt, Gakt_tex_12_fakt, Gakt_avarija_12_fakt FROM [GO_ћес€чный график работы источника] GO, [GT_ћес€чные температуры воздуха и грунта] GT, [»сточник тепла] IS3 WHERE IS3.kod_ist=GO.kod_ist


