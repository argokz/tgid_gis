select distinct obj.id, 
	   CONCAT ( ISNULL( obj.sredstvoDiag, 'средство диагностики не указано'), ' ', 
				ISNULL(FORMAT (obj.data_prov, 'dd-MM-yyyy'), 'дата не указана'), ' ',
				ISNULL(status, 'состояние не указано')) as name
from getPts_diag_by_sredstvoDiagID(0) as obj
join tkamera tk ON tk.shape.STDistance(obj.shape) < 0.1	
join linesobj l ON l.shape.STDistance(tk.shape) < 0.1
join nodes n ON n.id = l.nodeID1
where tk.id = $fileID$ and n.fileID in ($FragmentIds$)