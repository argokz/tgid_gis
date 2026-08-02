select 

n.id as id,
v.id as typ,
v.name as typ_name,
ec.name as kod, 
n.nodeName as uzel,
n.name as name,
CONCAT_WS(' ', ec.name, nodeName) as nodeName

from (
    select id, 1 as typ, externalCodeID, nodeName, '' as name from new_baza.uzel 
    union select id, 2, externalCodeID, nodeName, '' from new_baza.nasosn 
    union select id, 3, externalCodeID, nodeName, '' from new_baza.istoch 
    union select id, 4, externalCodeID, nodeName, name from new_baza.real_potr
    union select id, 5, externalCodeID, nodeName, name from new_baza.general_potr 
) n
join (
select * from  (

VALUES 
(1, 'Узел'),
(2, 'Насосная станция'),
(3, 'Источник тепла'),
(4, 'Потребитель реальный'),
(5, 'Потребитель обобщенный')
) AS v (id, name)

) v on v.id=n.typ
join externalcodes ec on ec.id=n.externalCodeID
--AND_TXT where CONCAT_WS(' ', v.name, ec.name, nodeName) ilike '%$txt$%'
