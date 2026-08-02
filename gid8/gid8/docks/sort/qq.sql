-- postgres
SELECT house_number
FROM houses
ORDER BY 
    NULLIF(regexp_replace(house_number, '\D.*$', ''), '')::int,  -- Числовая часть
    regexp_replace(house_number, '^\d+', '');                    -- Буквенная часть

-- ms sql

SELECT house_number
FROM houses
ORDER BY 
    TRY_CAST(SUBSTRING(house_number, 1, PATINDEX('%[^0-9]%', house_number + '0') - 1) AS INT), -- Числовая часть
    SUBSTRING(house_number, PATINDEX('%[^0-9]%', house_number + '0'), LEN(house_number))      -- Буквенная часть

-- sqlite

SELECT house_number
FROM houses
ORDER BY 
    CAST(SUBSTR(house_number, 1, INSTR(house_number || '0', '/') - 1) AS INTEGER), -- Числовая часть
    SUBSTR(house_number, INSTR(house_number || '0', '/') + 1);                   -- Буквенная часть
