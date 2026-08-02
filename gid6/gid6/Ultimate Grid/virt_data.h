#pragma once


enum VIRT_DATA {
  virt_data_unknown = 0,
  virt_data_node = 2,
  virt_data_line = 3,
  virt_data_line_big = 4,
  virt_data_geo = 5,
  virt_data_unknown_out = 6,
  virt_data_node_out = 7,
  virt_data_line_out = 8,
  virt_data_edit_add = 9,
  virt_data_edit_add_del = 10,
  virt_data_edit = 11,
  virt_data_tg = 12,
  virt_data_tu = 13,
  virt_data_del = 14,
  virt_data_edit_del = 15,
  virt_data_geo_big_line = 16,
  virt_data_line_big2 = 17,
  virt_data_geo_del = 18,
  virt_data_geo_edit = 19,
  virt_data_geo_noedit = 20,
  virt_data_line_big_osmotr = 22,
  virt_data_node_noedit = 23,
  virt_data_line_noedit = 24
};

bool isVTEdit(VIRT_DATA type);
bool isVTAdd(VIRT_DATA type);
bool isVTDel(VIRT_DATA type);
bool isVTEdit2(VIRT_DATA type);
