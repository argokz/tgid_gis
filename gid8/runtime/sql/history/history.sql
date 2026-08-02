select top 1000 
ch.dateRemoved as 'Дата', 
o.user_name as 'Оператор',
fr.name as 'Фрагмент',
chtypes.name as 'Функция',  
ec.name as 'Код РС', 
n.externalNodeName as 'Наименование узла',

ec1.name as 'Код РС 1', 
n1.externalNodeName as 'Наименование начального узла',
ec2.name as 'Код РС 2', 
n2.externalNodeName as 'Наименование конечного узла'

--,
--ch.* , * 
from changed_object ch
left join nodes n on n.id=ch.changedID and ch.changeTypeID in (2, 5, 7, 11, 13)
left join externalCodes ec on ec.id=n.externalCodeID

left join linesobj l on l.id=ch.changedID and ch.changeTypeID in (1, 6, 8, 10, 12)
left join nodes n1 on n1.id=l.nodeID1
left join nodes n2 on n2.id=l.nodeID2
left join externalCodes ec1 on ec1.id=n1.externalCodeID
left join externalCodes ec2 on ec2.id=n2.externalCodeID
--left join operators o on o.id=ch.userRemoved
left join passwords o on o.id=ch.userRemoved
left join fragments fr on fr.id=n.fileID or fr.id=n1.fileID

left join (

select * from (
values 
(15,'Добавлен объект геобазы'),
(10,'Добавлен участок'),
(11,'Добавлен узел'),
(3, 'Удалена схема'),
(4, 'Удалена схема'),
(14,'Удален объект геобазы'),
(1, 'Удален участок'),
(2, 'Удален узел'),
(9, 'Редактирование'),
(8, 'Редактирование узла'),
(7, 'Редактирование участка'),
(16,'Передвинут объект геобазы'), 
(6, 'Передвинут участок'),       
(5, 'Передвинут узел'),          
(12,'Изменен тип участка'),    
(13,'Изменен тип узла')
) as chtypes(id, name) 
) chtypes on chtypes.id=changeTypeID

order by ch.id desc

