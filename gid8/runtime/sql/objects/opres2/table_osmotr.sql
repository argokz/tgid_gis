select
	IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as '��������� ����',
	IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as '�������� ����',
	IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as '������������ �������',
	IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as '��������� �������',
	d.defectDescription as '�������� �������',
	obj.name as '������������',
	sost.name as '���������',
	obj.data_planiruemogo_osmotra as '���� ������������ �������',
	obj.data_osmotra as '���� �������',
	obj.mesto_kontrolnoy_vyrezki as '����� ����������� ������� ������������',
	obj.god_prokladki_teploseti as '��� ���������',
	tp.name as '��� ���������',
	obj.diametr_uslovnuy_p as '������� �������� �����',
	obj.tolshchina_stenki_p as '������� ������ �������� �����',
	obj.diametr_uslovnuy_o as '������� �������� �����',
	obj.tolshchina_stenki_o as '������� ������ �������� �����',
	obj.harakteristika as '�������������� �������������',
	obj.sostoyanie_izol as '��������� ��������',
	obj.nalichie_vneshney_korrozii_p as '������� ������� �������� �������� �����',
	obj.nalichie_vneshney_korrozii_o as '������� ������� �������� �������� �����',
	obj.nalichie_vnutrenney_korrozii_p as '������� ���������� �������� �������� �����',
	obj.nalichie_vnutrenney_korrozii_o as '������� ���������� �������� �������� �����',
	obj.planiruemye_meropriyatiya as '����������� �����������',
	obj.otvetstvennoe_lico_ID as '������������� ����',
	obj.data_zapolneniya as '���� ����������',
	obj.primechanie as '����������'
from osmotr obj
	left join tubingTypes tp on tp.id = obj.id
	left join osmotrDeployed osd ON osd.directionID = obj.id
	left join nachalniki_uchastkov nach ON nach.id=obj.otvetstvennoe_lico_ID
	left join osmotr_sostoyanie sost on sost.id = obj.sostoyanie_ID
	join linesobj l on l.id = osd.lineID
	LEFT JOIN heatPipцeSections hps ON hps.lineID = l.id
	LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID
	LEFT JOIN nodes n1 ON n1.id = srt.nodeID1
	LEFT JOIN nodes n2 ON n2.id = srt.nodeID2
	left join externalCodes ec1 ON ec1.id = n1.externalCodeID
	left join externalCodes ec2 ON ec2.id = n2.externalCodeID
	join defect d ON l.shape.STDistance(d.shape) < 0.1 and d.osmotrID = obj.id
	left join uchastok_ms ms ON ms.id = srt.uchastok_ms_id
	left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
	left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

	left join uchastok_rs rs ON rs.id = srt.uchastok_rs_id
	left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
	left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

