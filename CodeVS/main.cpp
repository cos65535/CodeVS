#include "Base.h"
#include "Field.h"
#include "Simulator.h"
#include "Structure.h"
#include "RappidPut.h"
#include "Tron.h"

//void AdjustTron(int map) {
//  int best = -100000;
//  int bestUse = -1;
//  int bestFrozen = -1;
//  Simulator simulator[10];
//  REP(iter, 10) {
//    char filename[100];
//    sprintf(filename, "inputs/input%d.txt", iter);
//    simulator[iter].Load(filename);
//  }
//  fprintf(stderr, "Stage:%d\n", map + 1);
//  int upper = 60;
//  FOR(use, 50, upper) {
//    int sum = 0;
//    vector<int> result;
//    REP(iter, 10) {
//      int money = 0;
//      REP(level, 25) {
//        vector<TowerInfo> output = Tron::TronAI(simulator[iter].stages[map], map, level, false, use, 0);
//        money += simulator[iter].LevelSimulation(map, level, output).second;
//      }
//      //fprintf(stderr, "Use: %d, Frozen: %d, Money: %d\n", use, frozen, money);
//      sum += money;
//    }
//    sum /= 10;
//    if (sum > best) {
//      upper = use + 30;
//      best = sum;
//      bestUse = use;
//      fprintf(stderr, "Update!!\n");
//      fprintf(stderr, "Use: %d, Money: %d\n\n", use, sum);
//    }
//  }
//  best = -100000;
//  upper = 20;
//  REP(frozen, upper) {
//    int sum = 0;
//    vector<int> result;
//    REP(iter, 10) {
//      int money = 0;
//      REP(level, 25) {
//        vector<TowerInfo> output = Tron::TronAI(simulator[iter].stages[map], map, level, false, bestUse, frozen);
//        money += simulator[iter].LevelSimulation(map, level, output).second;
//      }
//      //fprintf(stderr, "Use: %d, Frozen: %d, Money: %d\n", use, frozen, money);
//      sum += money;
//    }
//    sum /= 10;
//    if (sum > best) {
//      upper = frozen + 20;
//      best = sum;
//      bestFrozen = frozen;
//      fprintf(stderr, "Update!!\n");
//      fprintf(stderr, "Use: %d, Frozen: %d, Money: %d\n\n", bestUse, frozen, sum);
//    }
//  }
//  OutputLog("log.txt", "mapUse[%d]=%3d;mapFrozen[%d]=%2d;//Money=%d\n", map, bestUse, map, bestFrozen, -best);
//}

int Test(Simulator &simulator, int map) {
  pair<int, int> result;
  vector<TowerInfo> old;
  REP(level, 25) {
//    fprintf(stderr, "Stage:%d-%d\n", map + 1, level + 1);
    
    vector<TowerInfo> output;
    if (map < 40) {
      output = RappidPut::RappidPut2(simulator.stages[map], map, level, true);
      //output = RappidPut::ReplayAttack(simulator.stages[map], map, level);
    } else {
      output = Tron::TronAI(simulator.stages[map], map, level, true);
      //output = Tron::ReplayAttack(simulator.stages[map], map, level);
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
  return result.second;
}

int main() {
  srand(123456);
  Simulator simulator("inputs/input.txt");
#ifndef CONTEST
  int start = timeGetTime();
  //int sum = 100;
  //FOR(map, 0, 80) {
  //  if (map == 40) { sum /= 2; }
  //  sum += Test(simulator, map);
  //  printf("TotalMoney: %d\n", sum);
  //}
  FOR(iter, 4, 30) {
    printf("Iter:%d\n", iter);
    //int sum = 100;
    int sum = 529000;
    char filename[100];
    sprintf(filename, "inputs/input%d.txt", iter);
    Simulator simulator(filename);
    FOR(map, 40, 80) {
      if (map == 40) { sum /= 2; }
      sum += Test(simulator, map);
      printf("TotalMoney: %d\n", sum);
    }
  }
  //printf("Simulation Time: %d.%d\n", simulator.totalTime);
  int end = timeGetTime();
  printf("Total Time: %d(sec)\n", (end - start) / 1000);
  puts("Please Enter Key");
  getchar();
  return 0;
#endif

  int mapCnt;
  scanf("%d", &mapCnt);
  int plife = 10;
  int pmoney = 100;
  pair<int, int> ans(0, 0);
  REP(map, mapCnt) {
    MapInfo mapInfo;
    mapInfo.LoadHeader(stdin);
    memcpy(simulator.stages[map].field, mapInfo.field, sizeof(int) * 51 * 51);
    REP(level, mapInfo.levelCnt) {
      mapInfo.LoadLevel(stdin);
      //assert(mapInfo.levels[level].life == simulator.stages[map].levels[level].life);
      //assert(mapInfo.levels[level].money == simulator.stages[map].levels[level].money);

      //fprintf(stderr, "Stage:%d-%d\n", map + 1, level + 1);
      //fprintf(stderr, "Ans:%d %d\n", plife - mapInfo.levels.back().life, mapInfo.levels.back().money - pmoney);
      //if (map != 40 || level != 0) {
      //  assert(plife - ans.first == mapInfo.levels.back().life);
      //  assert(pmoney + ans.second == mapInfo.levels[level].money);
      //}
      //plife = mapInfo.levels.back().life;
      //pmoney = mapInfo.levels.back().money;

      vector<TowerInfo> output;
      if (map < 40) {
        output = RappidPut::ReplayAttack(simulator.stages[map], map, level);
      } else {
        output = Tron::ReplayAttack(simulator.stages[map], map, level);
      }

      // Print
      printf("%d\n", (int)output.size());
      FORIT(it, output) {
        printf("%d %d %d %d\n", it->x, it->y, it->level, it->type);
      }
      fflush(stdout);
      ans = simulator.LevelSimulation(map, level, output);

      //fprintf(stderr, "Simulator:%d %d\n", ans.first, ans.second);
    }
  }
}
