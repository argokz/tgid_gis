SELECT top 2147483647 
    id,
    specExpendID as 'kodur', 
--    calcHLcond as 'kondiz', 
--    calcHLcloseSys as 'txz', 

    calcHLdep as 'otoplz',
    calcHLindep as 'otopln', 
    calcHLventil as 'ventil', 

    calcExpendHWopen as 'gvo', 
    circHLosOpen as 'rez', 
    
    
    avgHLGVScloseParall as 'gvpr', 
    avgHLGVScloseMix as 'gvsm', 
    avgHLGVScloseConseq as 'gvps', 
    avgHLGVSclosePreON as 'gvpw', 

--    calcHLopenSys as 'txo', 
    avgHLGVScloseSummer as 'gvz_leto',
    avgHLGVSopenSummer as 'gvo_leto', 
    hSourceCode as 'kod_ist'
from specExpends se

WHERE fileID=$fileID$