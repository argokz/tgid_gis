#pragma once

struct VYD_LINES
{
public:

  void init(CGraph2 *graph, bool all, bool onlypodacha);
  void init2(CGraph2 *graph, bool all, bool onlypodacha);

  map<int, CLINE2*> map_line;
  map<int, CNode2*> map_node;

//  VYD_LINES(){};
};

void sort_line(CGraph2 *m_graph, bool is_mark);
