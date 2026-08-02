select 

n.id as id,
v.id as typ,
--fr.name as fr_name,
v.name as typ_name,
ec.name as kod, 
n.nodeName as uzel,
n.name as name,
CONCAT_WS(' ', ec.name, n.nodeName) as nodeName

from (
    select id, 1 as typ, externalCodeID, nodeName, fileID, shape, '' as name from new_baza.uzel 
    union select id, 2, externalCodeID, nodeName, fileID, shape, '' from new_baza.nasosn 
    union select id, 3, externalCodeID, nodeName, fileID, shape, '' from new_baza.istoch 
    union select id, 4, externalCodeID, nodeName, fileID, shape, name from new_baza.real_potr
    union select id, 5, externalCodeID, nodeName, fileID, shape, name from new_baza.general_potr 
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

join $table$ l on l.id=$id$
join fragments fr on fr.id=n.fileID

where 
    ST_Distance(n.shape, l.shape) < 500 and n.fileID=l.fileID
--AND_TXT and CONCAT_WS(' ', v.name, ec.name, nodeName) ilike '%$txt$%'
