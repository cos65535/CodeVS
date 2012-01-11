#include "Base.h"
#include "Field.h"
#include "Simulator.h"
#include "Structure.h"
#include "RappidPut.h"
#include "Tron.h"
#include "Final.h"

int Test(Simulator &simulator, int map) {
  pair<int, int> result;
  vector<TowerInfo> old;
  REP(level, 25) {
    //    fprintf(stderr, "Stage:%d-%d\n", map + 1, level + 1);

    vector<TowerInfo> output;
    if (map < 40) {
      //output = FirstHalf::RappidPut2(simulator.stages[map], map, level, true);
      output = FirstHalf::ReplayAttack(simulator.stages[map], map, level);
    } else {
      //output = SecondHalf::TronAI(simulator.stages[map], map, level, true);
      //output = SecondHalf::ReplayAttack(simulator.stages[map], map, level);
      if (map != 0 && map != 40) {
        output = Final::AI(simulator.stages[map], simulator.stages[map - 1], map, level);
      } else {
        output = Final::AI(simulator.stages[map], simulator.stages[map], map, level);
      }
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
  srand(1234567);
  Simulator simulator("inputs/input.txt");
#ifndef CONTEST
  int start = timeGetTime();
  int sum = 520000;
  FOR(map, 40, 80) {
    if (map == 40) { sum /= 2; }
    sum += Test(simulator, map);
    printf("TotalMoney: %d\n", sum);
  }
  //FOR(iter, 19, 30) {
  //  printf("Iter:%d\n", iter);
  //  //int sum = 100;
  //  int sum = 529000;
  //  char filename[100];
  //  sprintf(filename, "inputs/input%d.txt", iter);
  //  Simulator simulator(filename);
  //  FOR(map, 40, 80) {
  //    if (map == 40) { sum /= 2; }
  //    sum += Test(simulator, map);
  //    printf("TotalMoney: %d\n", sum);
  //  }
  //}
  //printf("Simulation Time: %d.%d\n", simulator.totalTime);
  int end = timeGetTime();
  printf("Total Time: %d.%d(sec)\n", (end - start) / 1000, (end - start) % 1000);
  puts("Please Enter Key");
  getchar();
  return 0;
#endif

  int mapCnt;
  int v = scanf("%d", &mapCnt);
  assert(v == 1);
  //int plife = 10;
  //int pmoney = 100;
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
        output = FirstHalf::ReplayAttack(simulator.stages[map], map, level);
      } else {
        output = SecondHalf::ReplayAttack(simulator.stages[map], map, level);
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
