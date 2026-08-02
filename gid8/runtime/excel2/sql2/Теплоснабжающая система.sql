SELECT top 2147483647 
 'ря' AS ts, hs.name, nasel_point, sz.name AS season, year, t_or, t_vr, t_vnew
  tx, 
--  tx_leto, 
  tn_god, tn_god_leto, tg_god, tg_god_leto, a 
FROM 
  heatSystem hs
LEFT JOIN seasons sz ON sz.id=hs.seasonID
