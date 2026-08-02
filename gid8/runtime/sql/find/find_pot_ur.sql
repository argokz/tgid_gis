select 

n.id, 

ec.name AS 'Код РС', 
n.externalNodeName AS 'Наименование узла', 
ISNULL (rc.name, ISNULL (gc.name, n.nodeName)) AS 'Описание',
--n.NodeName AS 'Наименование ПТС', 

IIF (rc.id IS NULL,
IIF (gc.id IS NULL,
N'Узел'
,N'Обобщенный потребитель')
,N'Реальный потребитель')

 AS 'Тип',

fr.name AS 'Фрагмент'
 


from nodes n
join externalCodes ec on ec.id=n.externalCodeID
JOIN fragments fr ON fr.id=n.fileID
left join realConsumers rc on rc.nodeID=n.id and rc.specExpendID is null
left join generalizedConsumers gc on gc.nodeID=n.id and gc.specExpendID is null
where n.removed=0 and not (rc.id is null and gc.id is null) 
and n.fileID IN ($fileID$)
--AND_NODE
