SELECT top 2147483647 
	f.name as 'file',
	calc.Tn, calc.date1, calc.name
from CALCULATION calc
join fragments f on
f.id = calc.fileID
