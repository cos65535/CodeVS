#include "Base.h"
#include "Field.h"
#include "Simulator.h"
#include "Structure.h"
#include "RappidPut.h"
#include "Tron.h"

void Test(Simulator &simulator, int map) {
  pair<int, int> result;
  vector<TowerInfo> old;
  REP(level, 25) {
//    fprintf(stderr, "Stage:%d-%d\n", map + 1, level + 1);
    
    vector<TowerInfo> output;
    if (map < 40) {
      output = RappidPut(simulator.stages[map], map, level);
    } else {
      output = Tron::TronAI(simulator.stages[map], map, level);
    }
    pair<int, int> nret = simulator.LevelSimulation(map, level, output);
    //FORIT(it, output) {
    //  old.push_back(*it);
    //}
    //printf("Ans %d: Damage:%d Money:%d\n", level + 1, nret.first, nret.second);
    result.first += nret.first;
    result.second += nret.second;
  }
  printf("Total %d: Damage:%d Money:%d\n", map + 1, result.first, result.second);
}

int main() {
  srand(123456789);
  //Simulator simulator("input.txt");
  //FOR(map, 0, 71 -1) {
  //  Test(simulator, map);
  //}
  //return 0;

  int mapCnt;
  scanf("%d", &mapCnt);
  int plife = 10;
  int pmoney = 100;
  pair<int, int> ans(0, 0);
  REP(map, mapCnt) {
    MapInfo mapInfo;
    mapInfo.LoadHeader(stdin);
    REP(level, mapInfo.levelCnt) {
      mapInfo.LoadLevel(stdin);

      //fprintf(stderr, "Stage:%d-%d\n", map + 1, level + 1);
      //fprintf(stderr, "Ans:%d %d\n", plife - mapInfo.levels.back().life, mapInfo.levels.back().money - pmoney);
      //if (map != 40 && level == 0) {
      //  assert(plife - ans.first == mapInfo.levels.back().life);
      //  assert(pmoney + ans.second == mapInfo.levels.back().money);
      //}
      //plife = mapInfo.levels.back().life;
      //pmoney = mapInfo.levels.back().money;

      vector<TowerInfo> output;
      if (map < 40) {
        output = RappidPut(mapInfo, map, level);
      } else {
        output = Tron::TronAI(mapInfo, map, level);
      }

      // Print
      printf("%d\n", (int)output.size());
      FORIT(it, output) {
        printf("%d %d %d %d\n", it->x, it->y, it->level, it->type);
      }
      fflush(stdout);

      //ans = simulator.LevelSimulation(map, level, output, mapInfo.levels[level].tower);
      //fprintf(stderr, "Simulator:%d %d\n", ans.first, ans.second);
    }
  }
}
