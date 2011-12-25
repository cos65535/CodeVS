#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"
#include "Mask.h"
#include "Simulator.h"

vector<TowerInfo> LevelDown(const MapInfo &mapInfo, const int map, vector<TowerInfo> iniTowers) {
  vector<TowerInfo> best = iniTowers;
  int index = 0;
  while (index < (int)best.size()) {
    vector<TowerInfo> temp = best;
    if (temp[index].level == 0) {
      temp.erase(temp.begin() + index);
    } else {
      temp[index].level--;
    }
    if (Simulator::MapSimulation(mapInfo, map, temp).first == 0) {
      best = temp;
    } else {
      index++;
    }
  }
  return best;
}