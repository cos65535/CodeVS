#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"
#include "Mask.h"
#include "Simulator.h"

vector<TowerInfo> LevelDown(const MapInfo &mapInfo, int map, vector<TowerInfo> iniTowers, int damage) {
  Simulator simulator;
  map = 0;
  simulator.stages.push_back(mapInfo);

  Field field(mapInfo);
  field.PutTower(iniTowers);
  int route[FS][FS];
  int mask[FS][FS];
  memset(mask, 0, sizeof(int) * FS * FS);
  field.CalcEnemyRoute(mask, route);

  vector<TowerInfo> best = iniTowers;
  int index = 0;
  int bestMoney = simulator.MapSimulation(false, map, best, 1).second;
  int dx[4] = { 1, 0, -1, 0 };
  int dy[4] = { 0, 1, 0, -1 };
  while (index < (int)best.size()) {
    vector<TowerInfo> temp = best;
    if (temp[index].level == 0) {
      int sum = 0;
      REP(dir, 4) {
        int nx = temp[index].x + dx[dir];
        int ny = temp[index].y + dy[dir];
        sum += route[ny][nx] > 0;
      }
      if (sum >= 2) { index++; continue; }
      temp.erase(temp.begin() + index);
    } else {
      if (map < 40) {
        index++;
        continue;
      }
      temp[index].level = 0;
    }
    pair<int, int> result = simulator.MapSimulation(false, mapInfo, map, temp, damage);
    if (result.first <= damage && result.second > bestMoney) {
      best = temp;
      bestMoney = result.second;
    } else {
      index++;
    }
  }
  return best;
}
