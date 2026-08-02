SELECT top 2147483647
    id,
    kodkv, kvpot, otoplz, otopln, ventil, kondiz, txz, txop, txoo, gvz, gvop, 
    gvoo, ut, cher, diam
from varCoefficients vc

WHERE fileID=$fileID$