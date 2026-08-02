SELECT kod, diametr, diametr_usl, y, kolwork, 'Á' AS name_typ, q_B_P AS q_P, q_B_O AS q_O FROM TEPLO_OUT 
UNION ALL SELECT kod, diametr, diametr_usl, y, kolwork, 'Ê', q_K_P, q_K_O FROM TEPLO_OUT 
UNION ALL SELECT kod, diametr, diametr_usl, y, kolwork, 'Í', q_N_P, q_N_O FROM TEPLO_OUT
