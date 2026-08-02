#include "stdafx.h"
#include "ado.h"
#include "gid6.h"

#include "graph2.h"
#include "geofile.h"

#include "gidrView.h"

#include "Filtr3.h"

#include "NewPassport/CWebDialog.h"

// Покаывать участки по опрессовке

void list_ms(CWnd *wnd, CString tn, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;
    
    CString q;

    q.Format(
"SELECT DISTINCT\n"
"ms.id,\n"
"nach.fio AS 'Начальник участка',\n"
"ue.nomer_uchastka AS 'Номер участка',\n"
"ms.opisanie_uchastka_ms AS 'Участок МС'\n"

"FROM %s r\n"
"JOIN %sDeployed depl ON depl.directionID=r.id\n"
"JOIN linesobj l ON l.id=depl.lineID AND l.removed=0\n"

"JOIN heatPipeSections hps ON hps.lineID=l.id \n"
"JOIN uchastok_ms ms ON ms.id=hps.magistralSite\n"
"JOIN uchastki_ekspluatatsii ue ON ue.id=ms.nomer_uchastka\n"
"JOIN nachalniki_uchastkov nach ON nach.id=ue.nachalnik_uchastka\n"
"WHERE r.id=%d\n"
"ORDER BY nach.fio, ms.opisanie_uchastka_ms"

, tn, tn, id);


    int n_ret = 0;

    CWebDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), 
        tn, q, n_ret, virt_data_unknown, "Описание магистральных сетей контура", true);

    dlg.DoModal();
}

void list_rs(CWnd* wnd, CString tn, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString q;

    q.Format(
        "SELECT DISTINCT\n"
        "rs.id,\n"
        "nach.fio AS 'Начальник участка',\n"
        "ue.nomer_uchastka AS 'Номер участка',\n"
        "rs.naimenovanie_uchastka_rs AS 'Участок РС'\n"

        "FROM %s r\n"
        "JOIN %sDeployed depl ON depl.directionID=r.id\n"
        "JOIN linesobj l ON l.id=depl.lineID AND l.removed=0\n"

        "JOIN heatPipeSections hps ON hps.lineID=l.id \n"
        "JOIN uchastok_rs rs ON rs.id=hps.distSite\n"
        "JOIN uchastki_ekspluatatsii ue ON ue.id=rs.nomer_uchastka\n"
        "JOIN nachalniki_uchastkov nach ON nach.id=ue.nachalnik_uchastka\n"
        "WHERE r.id=%d\n"
        "ORDER BY nach.fio, rs.naimenovanie_uchastka_rs"
        
        
        , tn, tn, id);


    int n_ret = 0;

    CWebDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(),
        tn, q, n_ret, virt_data_unknown, "Описание распределительных сетей контура ", true);

    dlg.DoModal();
}


void list_uch(CWnd* wnd, CString tn, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString q;

    q.Format(

"SELECT \n"
"id,\n"
"uzel1,\n"
"uzel2,\n"
"[Диаметр условный, мм],"
"[Протяженность, м]\n"
"FROM (\n"
"SELECT DISTINCT\n"
"pss.id,\n"
"IIF (n1.nodeName IS NULL OR n1.nodeName = '' OR n1.nodeName = ' ', n1.externalNodeName, n1.nodeName) AS uzel1,\n"
"IIF (n2.nodeName IS NULL OR n2.nodeName = '' OR n2.nodeName = ' ', n2.externalNodeName, n2.nodeName) AS uzel2,\n"

"pss.DiamUslov AS 'Диаметр условный, мм',"
"pss.pipeLength AS 'Протяженность, м',\n"
"srt.id AS srt_id\n"

"FROM %s r\n"
"JOIN %sDeployed depl ON depl.directionID=r.id\n"
"JOIN linesobj l ON l.id=depl.lineID AND l.removed=0\n"

"JOIN heatPipeSections hps ON hps.lineID=l.id \n"
"JOIN pipeSections pss ON pss.id=hps.pipeSectionID\n"
"LEFT JOIN sortLinesForUchastok srt ON srt.pipeSectionID=pss.id\n"
"JOIN nodes n1 ON n1.id=pss.nodeID1\n"
"JOIN nodes n2 ON n2.id=pss.nodeID2\n"

"WHERE r.id=%d\n"
") ___\n"
"ORDER BY srt_id\n"
        
        , tn, tn, id);

    int n_ret = 0;

    CWebDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(),
        tn, q, n_ret, virt_data_line_big, "Участки опрессовки", true);

    dlg.DoModal();
}




void CGidrView::view_nodes()
{
}

void list_PT(CGidrView *view, CAdoFile * ado, CWnd* wnd, CString tn, int id)
{

CString q;

q.Format(

"SELECT DISTINCT\n"
"n.id,\n"
"ec.name AS 'Код РС', \n"
"IIF (n.NodeName IS NOT NULL, n.NodeName, n.externalNodeName) AS 'Наименование узла',\n"
"IIF (gc.name IS NOT NULL, gc.name, rc.name) AS 'Описание',\n"
"IIF (ms.id IS NOT NULL, ms.opisanie_uchastka_ms,rs.naimenovanie_uchastka_rs) AS 'Участок',\n"
"--o.name AS 'Владелец',\n"
"fr.name AS 'Фрагмент',\n"
"srt.orderID AS '#'\n"

"FROM nodes n\n"
"LEFT JOIN sortNodesForUchastok srt ON srt.nodeID=n.id\n"
"JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
"JOIN fragments fr ON fr.id=n.fileID\n"
"LEFT JOIN generalizedConsumers gc ON gc.nodeID=n.id\n"
"LEFT JOIN realConsumers rc ON rc.nodeID=n.id\n"
"JOIN linesobj l ON l.nodeID1=n.id OR l.nodeID2=n.id\n"
"JOIN opresDeployed dep ON dep.lineID=l.id\n"
"JOIN opres obj ON obj.id=dep.directionID\n"
"LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"LEFT JOIN uchastok_rs rs ON rs.id=hps.distSite\n"
"LEFT JOIN uchastok_ms ms ON ms.id=hps.magistralSite\n"
"LEFT JOIN organizations o ON o.id=l.organizationID\n"

"WHERE n.removed=0 AND n.internalNodeID IS NULL\n"
"AND obj.id=%d\n"
//"AND rs.id IS NULL AND (gc.id IS NOT NULL OR rc.id IS NOT NULL)\n"
"AND (gc.id IS NOT NULL OR rc.id IS NOT NULL)\n"
"ORDER BY srt.orderID", id);

    int n_ret = 0;

    CWebDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(),
        tn, q, n_ret, virt_data_node, "Список подключаемых потребителей к контуру", true);

    dlg.DoModal();
}


void list_US(CGidrView *view, CAdoFile * ado, CWnd* wnd, CString tn, int id, CString tnl, CString title)
{

CString q;

//    CString tnl = "list_opres_node1";


q.Format(
"SELECT DISTINCT TOP 2000000\n"
"n.id,\n"
"n.NodeName,\n"
"lst.nodeID,\n"
"srt.orderID\n"

"FROM nodes n\n"
"LEFT JOIN sortNodesForUchastok srt ON srt.nodeID=n.id\n"
"JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
"JOIN fragments fr ON fr.id=n.fileID\n"
"LEFT JOIN realConsumers rc ON rc.nodeID=n.id\n"
"JOIN linesobj l ON l.nodeID1=n.id OR l.nodeID2=n.id\n"
"JOIN opresDeployed dep ON dep.lineID=l.id\n"
"JOIN opres obj ON obj.id=dep.directionID\n"
"LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"LEFT JOIN %s lst ON lst.nodeID = n.id AND lst.objID = obj.id\n"

"WHERE n.removed=0 AND n.internalNodeID IS NULL\n"
"AND obj.id=%d\n"
"AND n.nodeName IS NOT NULL \n"
"AND n.nodeName <> ''\n"
"ORDER BY lst.nodeID DESC, srt.orderID\n"
, tnl, id);


q.Format(
"SELECT DISTINCT TOP 2000000\n"
"n.id,\n"
"n.NodeName,\n"
"lst.nodeID\n"

"FROM nodes n\n"
"JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
"JOIN fragments fr ON fr.id=n.fileID\n"
"LEFT JOIN realConsumers rc ON rc.nodeID=n.id\n"
"JOIN linesobj l ON l.nodeID1=n.id OR l.nodeID2=n.id\n"
"JOIN opresDeployed dep ON dep.lineID=l.id\n"
"JOIN opres obj ON obj.id=dep.directionID\n"
"LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"LEFT JOIN %s lst ON lst.nodeID = n.id AND lst.objID = obj.id\n"

"WHERE n.removed=0 AND n.internalNodeID IS NULL\n"
"AND obj.id=%d\n"
"AND n.nodeName IS NOT NULL \n"
"AND n.nodeName <> ''\n"
"ORDER BY lst.nodeID DESC"
, tnl, id);





    CFiltr3 dlg(wnd, title);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            CString s = ado->readStr(1);
            int on = ado->read_long(2);
            dlg.Add(s, id, on > 0);
            ado->MoveNext();
        }
    }

    if (dlg.DoModal() != IDOK) return;

    q.Format("DELETE FROM %s WHERE objID=%d", tnl, id);

    
    long affected;
    bool ret = ado->Execute(q, &affected);
    
    CAdoInsert ins(ado, tnl, "objID,nodeID");

    for (auto it : dlg.map_item) {
        if (it.second.on) {
           if (ins.ado_insert_int2(id, it.second.id)) ret = true;
        }
    }
    ins.ado_insert();


//    int n_ret = 0;

//    CWebDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(),
//        tn, q, n_ret, virt_data_node, "Список подключаемых потребителей к контуру", true);

//    dlg.DoModal();
}