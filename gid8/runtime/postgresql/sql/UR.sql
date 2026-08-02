SELECT 
    id,
    specExpendID as kodur, calcHLcond as kondiz, calcHLcloseSys as txz, calcHLdep as otoplz,
    calcHLindep as otopln, calcHLventil as ventil, avgHLGVScloseParall as gvpr, avgHLGVScloseMix as gvsm, avgHLGVScloseConseq as gvps, 
    avgHLGVSclosePreON as gvpw, circHLosOpen as rez, calcExpendHWopen as gvo, calcHLopenSys as txo, avgHLGVScloseSummer as gvz_leto,
    avgHLGVSopenSummer as gvo_leto, hSourceCode as kod_ist
from specExpends se

WHERE fileID=$fileID$