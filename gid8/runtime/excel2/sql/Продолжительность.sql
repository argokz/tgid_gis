SELECT DISTINCT
  GO2.kod_ist,
  GO2.length_1+GO2.length_2+GO2.length_3+GO2.length_4+GO2.length_5+GO2.length_6+GO2.length_7+GO2.length_8+GO2.length_9+GO2.length_10+GO2.length_11+GO2.length_12 AS length,
  GO2.length_1+GO2.length_2+GO2.length_3+
  DAY(CT.end_year)-1+GO2.length_10-DAY(CT.begin_year)+
  GO2.length_11+GO2.length_12 AS lengthZ,
  GO2.length_4-DAY(CT.end_year)+1+GO2.length_5+GO2.length_6+GO2.length_7+GO2.length_8+GO2.length_9+DAY(CT.begin_year) AS lengthL,
  (GO2.length_1*GO2.t1_1+GO2.length_2*GO2.t1_2+GO2.length_3*GO2.t1_3+GO2.length_4*GO2.t1_4+GO2.length_5*GO2.t1_5+GO2.length_6*GO2.t1_6+GO2.length_7*GO2.t1_7+GO2.length_8*GO2.t1_8+GO2.length_9*GO2.t1_9+GO2.length_10*GO2.t1_10+GO2.length_11*GO2.t1_11+GO2.length_12*GO2.t1_12)/length AS t1srg,
  (GO2.length_1*GO2.t2_1+GO2.length_2*GO2.t2_2+GO2.length_3*GO2.t2_3+GO2.length_4*GO2.t2_4+GO2.length_5*GO2.t2_5+GO2.length_6*GO2.t2_6+GO2.length_7*GO2.t2_7+GO2.length_8*GO2.t2_8+GO2.length_9*GO2.t2_9+GO2.length_10*GO2.t2_10+GO2.length_11*GO2.t2_11+GO2.length_12*GO2.t2_12)/length AS t2srg,
  (GO2.length_1*GO2.t_1+GO2.length_2*GO2.t_2+GO2.length_3*GO2.t_3+GO2.length_4*GO2.t_4+GO2.length_5*GO2.t_5+GO2.length_6*GO2.t_6+GO2.length_7*GO2.t_7+GO2.length_8*GO2.t_8+GO2.length_9*GO2.t_9+GO2.length_10*GO2.t_10+GO2.length_11*GO2.t_11+GO2.length_12*GO2.t_12)/length AS tsrg,

  ((GO2.length_4-DAY(CT.end_year)+1)*GO2.t1_4+GO2.length_5*GO2.t1_5+GO2.length_6*GO2.t1_6+GO2.length_7*GO2.t1_7+GO2.length_8*GO2.t1_8+GO2.length_9*GO2.t1_9+DAY(CT.begin_year)*GO2.t1_10)/length AS t1srg_L

  
FROM
[GO_Месячный график работы источника] GO2,
[Система теплоснабжения] CT,
[Источник тепла] IS4
WHERE IS4.kod_ist=GO2.kod_ist
