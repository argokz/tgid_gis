#include <map>

static std::map<int, int> map_CurrentExternalCode;
static std::map<int, int> map_ur; // Удельные расходы

void initDefault()
{
  map_CurrentExternalCode.clear();
  map_ur.clear();
}


int getCurrentExternalCode(int fileID)
{
  auto it = map_CurrentExternalCode.find(fileID);
  if (it != map_CurrentExternalCode.end()) {
    return it->second;
  }
  
  return 0;
}

void setCurrentExternalCode(int fileID, int kod)
{
  map_CurrentExternalCode[fileID] = kod;
}



int get_ur(int fileID)
{
  auto it = map_ur.find(fileID);
  if (it != map_ur.end()) {
    return it->second;
  }
  
  return 0;
}

void set_ur(int fileID, int kod)
{
  map_ur[fileID] = kod;
}
