SELECT 
  kod_ist, M, MON, period,
  tn_long, 
  tgr_long, 
  length, 
  length AS len, 
  t1, 
  t2,
  t,
  length_fakt, 
  length_fakt AS lenf,
  tn_fakt, 
  tgr_fakt, 
  t1_fakt, 
  t2_fakt,
  t_fakt,
  Gpod_fakt, Ggvs_pribor_fakt, Gakt_tex_fakt, Gakt_avarija_fakt 

FROM TEMP_IST_OUT

UNION ALL

SELECT 
  kod_ist, 20-period, 'Средний', period,
  sum(tn_long*length)/len, 
  sum(tgr_long*length)/len, 
  sum(length) AS len1, 
  IIF(len1=0, 1, len1) AS len,
  sum(t1*length)/len,
  sum(t2*length)/len,
  sum(t*length)/len,
  sum(length_fakt) AS lenf1, 
  IIF(lenf1=0, 1, lenf1) AS len_fakt,
  sum(tn_fakt*length_fakt)/len_fakt,
  sum(tgr_fakt*length_fakt)/len_fakt,
  sum(t1_fakt*length_fakt)/len_fakt,
  sum(t2_fakt*length_fakt)/len_fakt,
  sum(t_fakt*length_fakt)/len_fakt,
  sum(Gpod_fakt*length_fakt)/len_fakt,
  sum(Ggvs_pribor_fakt*length_fakt)/len_fakt,
  sum(Gakt_tex_fakt*length_fakt)/len_fakt,
  sum(Gakt_avarija_fakt*length_fakt)/len_fakt

FROM TEMP_IST_OUT

GROUP BY kod_ist, period

UNION ALL

SELECT 
  kod_ist, 30, 'Средний', 2,
  sum(tn_long*length)/len, 
  sum(tgr_long*length)/len, 
  sum(length) AS len1, 
  IIF(len1=0, 1, len1) AS len,
  sum(t1*length)/len, 
  sum(t2*length)/len,
  sum(t*length)/len,
  sum(length_fakt) AS lenf1, 
  IIF(lenf1=0, 1, lenf1) AS len_fakt,
  sum(tn_fakt*length_fakt)/len_fakt,
  sum(tgr_fakt*length_fakt)/len_fakt,
  sum(t1_fakt*length_fakt)/len_fakt,
  sum(t2_fakt*length_fakt)/len_fakt,
  sum(t_fakt*length_fakt)/len_fakt,
  sum(Gpod_fakt*length_fakt)/len_fakt,
  sum(Ggvs_pribor_fakt*length_fakt)/len_fakt,
  sum(Gakt_tex_fakt*length_fakt)/len_fakt,
  sum(Gakt_avarija_fakt*length_fakt)/len_fakt

FROM TEMP_IST_OUT

GROUP BY kod_ist
