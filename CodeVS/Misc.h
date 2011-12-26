#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"
#include "Mask.h"
#include "Simulator.h"

vector<TowerInfo> LevelDown(const MapInfo &mapInfo, const int map, vector<TowerInfo> iniTowers, int damage) {
  vector<TowerInfo> best = iniTowers;
  int index = 0;
  int bestMoney = Simulator::MapSimulation(mapInfo, map, best).second;
  while (index < (int)best.size()) {
    vector<TowerInfo> temp = best;
    if (temp[index].level == 0) {
      temp.erase(temp.begin() + index);
    } else {
      temp[index].level--;
    }
    pair<int, int> result = Simulator::MapSimulation(mapInfo, map, temp);
    if (result.first <= damage && result.second > bestMoney) {
      best = temp;
      bestMoney = result.second;
    } else {
      index++;
    }
  }
  return best;
}