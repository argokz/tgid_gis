SELECT
TEMPL.kod_ist,
lengthZ,
lengthRZ,
t1Z,
t2Z,
tZ,
lengthL,
lengthRL,
t1L,
t2L,
tL,
lengthA,
lengthRA,
t1A,
t2A,
tA,
tnZ,
tgrZ,
tnL,
tgrL,
tnA,
tgrA,

lengthZ_fakt,
lengthRZ_fakt,
t1Z_fakt,
t2Z_fakt,
tZ_fakt,
lengthL_fakt,
lengthRL_fakt,
t1L_fakt,
t2L_fakt,
tL_fakt,
lengthA_fakt,
lengthRA_fakt,
t1A_fakt,
t2A_fakt,
tA_fakt,
tnZ_fakt,
tgrZ_fakt,
tnL_fakt,
tgrL_fakt,
tnA_fakt,
tgrA_fakt,


0.99 AS ro


FROM 
(

SELECT
kod_ist,
sum(length) AS lengthZ,
sum(lengthR) AS lengthRZ,
sum(length*t1)/sum(length) AS t1Z,
sum(length*t2)/sum(length) AS t2Z,
sum(length*t)/sum(length) AS tZ,
sum(length*tn_long)/sum(length) AS tnZ,
sum(length*tgr_long)/sum(length) AS tgrZ,

sum(length_fakt) AS lengthZ_fakt,
sum(lengthR_fakt) AS lengthRZ_fakt,
sum(length_fakt*t1_fakt)/sum(length_fakt) AS t1Z_fakt,
sum(length_fakt*t2_fakt)/sum(length_fakt) AS t2Z_fakt,
sum(length_fakt*t_fakt)/sum(length_fakt) AS tZ_fakt,
sum(length_fakt*tn_fakt)/sum(length_fakt) AS tnZ_fakt,
sum(length_fakt*tgr_fakt)/sum(length_fakt) AS tgrZ_fakt

FROM TEMP_IST_OUT
WHERE period=1
GROUP BY kod_ist
) TEMPL,
(

SELECT
kod_ist,
sum(length) AS lengthL,
sum(lengthR) AS lengthRL,
sum(length*t1)/sum(length) AS t1L,
sum(length*t2)/sum(length) AS t2L,
sum(length*t)/sum(length) AS tL,
sum(length*tn_long)/sum(length) AS tnL,
sum(length*tgr_long)/sum(length) AS tgrL,

sum(length_fakt) AS lengthL_fakt,
sum(lengthR_fakt) AS lengthRL_fakt,
sum(length_fakt*t1_fakt)/sum(length_fakt) AS t1L_fakt,
sum(length_fakt*t2_fakt)/sum(length_fakt) AS t2L_fakt,
sum(length_fakt*t_fakt)/sum(length_fakt) AS tL_fakt,
sum(length_fakt*tn_fakt)/sum(length_fakt) AS tnL_fakt,
sum(length_fakt*tgr_fakt)/sum(length_fakt) AS tgrL_fakt



FROM TEMP_IST_OUT
WHERE period=0
GROUP BY kod_ist
) TEMPZ,
(

SELECT
kod_ist,
sum(length) AS lengthA,
sum(lengthR) AS lengthRA,
sum(length*t1)/sum(length) AS t1A,
sum(length*t2)/sum(length) AS t2A,
sum(length*t)/sum(length) AS tA,
sum(length*tn_long)/sum(length) AS tnA,
sum(length*tgr_long)/sum(length) AS tgrA,

sum(length_fakt) AS lengthA_fakt,
sum(lengthR_fakt) AS lengthRA_fakt,
sum(length_fakt*t1_fakt)/sum(length_fakt) AS t1A_fakt,
sum(length_fakt*t2_fakt)/sum(length_fakt) AS t2A_fakt,
sum(length_fakt*t_fakt)/sum(length_fakt) AS tA_fakt,
sum(length_fakt*tn_fakt)/sum(length_fakt) AS tnA_fakt,
sum(length_fakt*tgr_fakt)/sum(length_fakt) AS tgrA_fakt


FROM TEMP_IST_OUT
GROUP BY kod_ist
) TEMPA

WHERE 
  TEMPL.kod_ist = TEMPZ.kod_ist
AND
  TEMPL.kod_ist = TEMPA.kod_ist