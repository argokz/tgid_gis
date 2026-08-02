#pragma once

  
  menuitem menudoc[] = {
    /* Байпас */ ST_BP, ST_BP, NULL, 2,
    /* Водоразборный прибор */ ST_VP, ST_VP, NULL, 1,
    /* Диафрагма */ ST_DR, ST_DR, NULL, 2,
    /* Задвижка */ ST_ZD, ST_ZD, NULL, 2,
//    /* Задвижки для переключения */ ST_ZD, "Задвижки для переключения", "select * from [Задвижка] where [name_zd]='1'",
    /* Здание */ ST_ZE, ST_ZE, NULL, 0,
    /* Источник тепла */ ST_IS, ST_IS, NULL, 1,
    /* Калориферная установка */ ST_KU, ST_KU, NULL, 2,
    /* Канал */ ST_KL, ST_KL, NULL, 0,
    /* Компенсатор */ ST_KM, ST_KM, NULL, 2,
    /* Коэффициенты вариации */ ST_KV, ST_KV, NULL, 0,
    /* Манометр */ ST_MT, ST_MT, NULL, 2,
    /* Местные сопротивления участка трубопровода */ ST_V1, ST_V1, "SELECT * FROM [Местные сопротивления участка трубопровода]", 0,
    /* Насосная станция */ ST_HS, ST_HS, NULL, 1,

    /* Клапан трехходовой */ ST_C3, ST_C3, NULL, 1,
   
    
    /* Насосный агрегат */ ST_HC, ST_HC, NULL, 2,
    /* Обратный клапан */ ST_OK, ST_OK, NULL, 2,
    /* Объекты с измеряемыми параметрами */ "Объекты с измеряемыми параметрами","Объекты с измеряемыми параметрами", NULL, 1,
    /* Опора */ ST_OP, ST_OP, NULL, 0,
    /* Организация */ ST_OR, ST_OR, NULL, 0,
    /* Потребитель обобщенный */ ST_PO, ST_PO, NULL, 1,
    /* Потребитель реальный */ ST_PR, ST_PR, NULL, 1,

    ST_PRIBOR, ST_PRIBOR, NULL, 0,


    /* Радиаторы системы отопления */ ST_RA, ST_RA, NULL, 2,
    /* Район эксплуатации */ ST_RN, ST_RN, NULL, 0,
    /* Расчетная схема */ ST_PC, ST_PC, NULL, 0,
    /* Регулирующая арматура                       */ ST_ZD2, ST_ZD2, NULL, 2,
    /* Регулятор давления */ ST_RD, ST_RD, NULL, 2,
    /* Регулятор перепада давления */ ST_RP, ST_RP, NULL, 2,
    /* Регулятор расхода */ ST_RR, ST_RR, NULL, 2,
    /* Система теплоснабжения */ ST_CT, ST_CT, NULL, 0,
    /* Теплообменник пластинчатый */ ST_TL, ST_TL, NULL, 2,
    /* Теплообменник трубчатый */ ST_TT, ST_TT, NULL, 2,
    /* Теплообменник */ ST_TP, ST_TP, NULL, 2,
    /* Термометр */ ST_TE, ST_TE, NULL, 1,
    /* Удельные расходы */ ST_UR, ST_UR, NULL, 0,
    /* Узел подпитки */ ST_UP, ST_UP, NULL, 1,
    /* Узел присоединения */ ST_US2,ST_US2,NULL, 1,
    /* Узел с заданным напором */ ST_ZN, ST_ZN, NULL, 1,
    /* Узел */ ST_US, ST_US, NULL/*"select * from [Узел] where [uzel_p] is NULL"*/, 1,
    /* Участок теплопровода */ ST_UT, ST_UT, NULL/*"select * from [Участок теплопровода] where [uzel_p] is NULL"*/, 2,
    /* ЦТП */ ST_ZP, ST_ZP, NULL, 1,
    /* Элеватор */ ST_EL, ST_EL, NULL, 2,
    NULL, NULL, NULL, 0,
  };


//    ST_ZD, "Задвижки для переключения", "select * from [Задвижка] where [name_zd]='Да'",

  menuitem menudocres[] = {

  ST_PT_RES, ST_PT_RES_NAME, NULL, 1,
  ST_DR_RES, ST_DR_RES_NAME, NULL, 1,
  ST_US_RES, ST_US_RES_NAME, NULL, 1,
  ST_UT_RES, ST_UT_RES_NAME, NULL, 2,
  ST_RS_RES, ST_RS_RES_NAME, NULL, 2,
  ST_NS_RES, ST_NS_RES_NAME, NULL, 2,

//  "ITOG2_OUT", "Запрос для Надежды Ивановны", "file:sql2/potreb.sql", 0,
  ST_BP_RES, ST_BP_RES_NAME, NULL, 2,
//  ST_DR_RES, ST_DR_RES_NAME, "select * from [DR_OUT]", 1,
//  ST_DR_RES, ST_DR_RES_NAME, NULL, 2,
  ST_ZD_RES, ST_ZD_RES_NAME, NULL, 2,
  ST_ZD2_RES, ST_ZD2_RES_NAME, NULL, 2,
//  ST_PT_RES, ST_PTOFF_RES_NAME, "select * from [PT_OUT] where [uzel] in (select [uzel] from [Потребитель реальный] where sost = '*')", 1,
//  "RPP_OUT", "Регуляторы перепада давления в потребителях", NULL, 2,
  "ITOG_OUT", "Теплопотребление заданное", "sql2/itog.sql", 0,
//  "ITOG2_OUT", "Теплопотребление общее", "excel2/sql2/OUT_Расчетные6.sql", 999,

  ST_OK_RES, ST_OK_RES_NAME, NULL, 2,
  ST_DRO_RES, ST_DRO_RES_NAME, NULL, 2,
  NULL, NULL, NULL, 0,
  };


//Q_fact_rasch

//[G]*[tP]-[G_obr]*[tO] as Q_fact_rasch

  menuitem menudocres2[] = {
  "Расчетные параметры", "Расчетные параметры", "select [nomer],[kod_p],[uzel_p],[pr_p],[kod],[uzel],[pr],[kod_ist],[name_object],[typ],[G],[Gutech],[Ggvs],[Gpodp],[pP],[pO],[t1ras],[t2ras],[Qot_treb],[Qgvz_treb],[Qgvo_treb],[Qvent_treb],[Q_treb],[poteri],[ispoln],[data] from [RASPAR_OUT]", 0,
//  "Анализ работы системы теплоснабжения", "Анализ работы системы теплоснабжения", "select [nomer],[kod_p],[uzel_p],[pr_p],[kod],[uzel],[pr],[kod_ist],[name_object],[typ],[G],[G_obr],[G]-[G_obr] as G_podpitki,[pP],[pO],[deltaP],[tP],[tO],[G_pod_fakt],[G_obr_fakt],[Gpodp_fakt],[G_pod_otkl],[G_obr_otkl],[Gpodp_otkl],[pP_fakt],[pO_fakt],[deltaP_fakt],[pP_pod_otkl],[pO_obr_otkl],[deltaP_otkl],[tP_fakt],[tO_fakt],[tP_pod_otkl],[tO_obr_otkl],[Q_treb],[Q_fact_rasch],[Q_fakt],[Q_otkl_treb],[Q_otkl_fact_rasch],[ispoln],[data] from [RASPAR_OUT]", 0,
  "Анализ работы системы теплоснабжения", "Анализ работы системы теплоснабжения", "select [nomer],[kod_p],[uzel_p],[pr_p],[kod],[uzel],[pr],[kod_ist],[name_object],[typ],[G],[G_obr],[G]-[G_obr] as G_podpitki,[pP],[pO],[deltaP],[tP],[tO],[G_pod_fakt],[G_obr_fakt],[Gpodp_fakt],[G_pod_otkl],[G_obr_otkl],[Gpodp_otkl],[pP_fakt],[pO_fakt],[deltaP_fakt],[pP_pod_otkl],[pO_obr_otkl],[deltaP_otkl],[tP_fakt],[tO_fakt],[tP_pod_otkl],[tO_obr_otkl],([G]*[tP]-[G_obr]*[tO])/1000 as Q_fact_rasch,[Q_fakt],[Q_otkl_treb],[Q_otkl_fact_rasch],[ispoln],[data] from [RASPAR_OUT]", 0,
  "Рассчитанные параметры","Рассчитанные параметры", "select [nomer],[kod_p],[uzel_p],[pr_p],[kod],[uzel],[pr],[kod_ist],[name_object],[typ],[G],[G_obr],[G]-[G_obr] as G_podpitki, [pP],[pO],[deltaP],[tP],[tO],[Q_fact_rasch],[ispoln],[data] from [RASPAR_OUT]", 0,
  "Фактические параметры", "Фактические параметры", "select [nomer],[kod_p],[uzel_p],[pr_p],[kod],[uzel],[pr],[kod_ist],[name_object],[typ],[G_pod_fakt],[G_obr_fakt],[Gpodp_fakt],[pP_fakt],[pO_fakt],[deltaP_fakt],[tP_fakt],[tO_fakt],[Q_fakt],[ispoln],[data] from [RASPAR_OUT]", 0,
  "Отклонения", "Отклонения", "select [nomer],[kod_p],[uzel_p],[pr_p],[kod],[uzel],[pr],[kod_ist],[name_object],[typ],[G_pod_otkl],[G_obr_otkl],[Gpodp_otkl],[pP_pod_otkl],[pO_obr_otkl],[deltaP_otkl],[tP_pod_otkl],[tO_obr_otkl],[Q_otkl_fact_rasch],[Q_otkl_treb],[ispoln],[data] from [RASPAR_OUT]", 0,
  NULL, NULL, NULL, 0,
  };
