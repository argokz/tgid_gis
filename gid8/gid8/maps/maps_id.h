#pragma once

enum
{
  ID_NOMAP = 0,
  ID_GOOGLE_MAP,
  ID_GOOGLE_SAT,
  ID_YANDEX_MAP,
  ID_YANDEX_SAT,
  ID_YANDEX_PMAP,
  ID_WMS_MAP,
  ID_GOOGLE_HYBRID,
  ID_2GIS_MAP,
  ID_VISICOM_MAP,
  ID_MAP_SEMEY,
  ID_MAP_SEMEY_TS,
  ID_OPENSTREETMAP,
  ID_ESRI_SATELLITE,

  // MapTiler. Новые значения добавляются ТОЛЬКО в конец: выбранная
  // карта хранится в настройках числом ("flags/internetMap"), и вставка
  // в середину молча поменяла бы смысл сохранённого значения — у всех,
  // кто уже пользуется программой, подложка сменилась бы сама собой.
  ID_MAPTILER_STREETS,
  ID_MAPTILER_TOPO,
  ID_MAPTILER_HYBRID,
  ID_MAPTILER_OSM
};

