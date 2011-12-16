#include "Base.h"
#include "Field.h"
#include "Simulator.h"
#include "Structure.h"
#include "RappidPut.h"

int main() {
  srand(123456789);
  Simulator simulator("input.txt");
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
      fprintf(stderr, "Stage:%d-%d\n", map + 1, level + 1);
      fprintf(stderr, "Ans:%d %d\n", plife - mapInfo.levels.back().life, mapInfo.levels.back().money - pmoney);
      if (map != 40 && level == 0) {
        assert(plife - ans.first == mapInfo.levels.back().life);
        assert(pmoney + ans.second == mapInfo.levels.back().money);
      }
      plife = mapInfo.levels.back().life;
      pmoney = mapInfo.levels.back().money;

      vector<TowerInfo> output;
      output = RappidPut(Field(mapInfo.field, mapInfo.w, mapInfo.h), map, level, mapInfo.levels.back().tower, mapInfo.levels.back().money);

      // Print
      printf("%d\n", (int)output.size());
      FORIT(it, output) {
        printf("%d %d %d %d\n", it->x, it->y, it->level, it->type);
      }
      fflush(stdout);
      ans = simulator.LevelSimulation(map, level, output, mapInfo.levels[level].tower);
      fprintf(stderr, "Simulator:%d %d\n", ans.first, ans.second);
    }
  }
}
