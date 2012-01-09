#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"

int CalcUse(const Field &field, const int mask[51][51]) {
  int ret = 0;
  REP(y, field.h) {
    REP(x, field.w) {
      ret += mask[y][x] != 0;
    }
  }
  return ret;
}

void PrintMask(const Field &field, const int mask[51][51]) {
  printf("%d %d\n", field.CalcDist(mask), CalcUse(field, mask));
  REP(y, field.h) {
    REP(x, field.w) {
      if (field.field[y][x] != '0') {
        int v = field.field[y][x];
        if (v == 1100) { v = '2'; }
        if (v == 1500) { v = '3'; }
        if (v >= 3000) { v = 'F'; }
        printf("%c", v);
      } else if (mask[y][x] != 0) {
        int v = '2';
        if (mask[y][x] == 15) { v = '3'; }
        if (mask[y][x] >= 30) { v = 'F'; }
        printf("%c", v);
      } else {
        printf(".");
      }
    }
    puts("");
  }
}

void SaveMask(const char *filename, const MapInfo &mapInfo, const vector<TowerInfo> &towers, int money, bool afterHalf) {
  Field field(mapInfo.field, mapInfo.w, mapInfo.h);
  field.PutTower(towers);
  FILE *fp = fopen(filename, "a");
  assert(fp != NULL);
  fprintf(fp, "%d\n", money);
  REP(y, field.h) {
    REP(x, field.w) {
      int type = field.field[y][x] / 1000 - 1;
      int level = field.field[y][x] % 1000 / 100 - 1;
      if (afterHalf && field.field[y][x] == '1') { type = 0; level = 0; }
      if (type < 0) {
        fprintf(fp, "0 ");
      } else {
        fprintf(fp, "%d ", (type + 1) * 10 + level + 1);
      }
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  fclose(fp);
}

struct MaskInfo {
  int money;
  int mask[51][51];
  MaskInfo() {;}
  MaskInfo(const MaskInfo &rhs) {
    money = rhs.money;
    memcpy(mask, rhs.mask, sizeof(int) * 51 * 51);
  }
  MaskInfo &operator=(const MaskInfo &rhs) {
    money = rhs.money;
    memcpy(mask, rhs.mask, sizeof(int) * 51 * 51);
    return *this;
  }
  bool operator<(const MaskInfo &rhs) const { return money < rhs.money; }
};
vector<MaskInfo> LoadMasks(const char *filename, const MapInfo &mapInfo) {
  Field field(mapInfo.field, mapInfo.w, mapInfo.h);
  int h = mapInfo.h;
  int w = mapInfo.w;
  FILE *fp = fopen(filename, "r");
  assert(fp != NULL);
  vector<MaskInfo> ret;
  int money;
  while (fscanf(fp, "%d", &money) > 0) {
    MaskInfo info;
    info.money = money;
    REP(y, h) {
      REP(x, w) {
        fscanf(fp, "%d", &info.mask[y][x]);
      }
    }
    if (field.OK2(info.mask)) {
      ret.push_back(info);
    }
  }
  sort(ret.rbegin(), ret.rend());
  return ret;
}

vector<TowerInfo> MaskToTower(const Field &field, const int mask[51][51], int money) {
  vector<TowerInfo> ret;
  REP(y, field.h) {
    REP(x, field.w) {
      if (field.field[y][x] >= 1500) { continue; }
      if (field.field[y][x] == '1' || field.field[y][x] == 's' || field.field[y][x] == 'g') { continue; }
      int type = mask[y][x] / 10 - 1;
      int level = mask[y][x] % 10 - 1;
      if (type >= 0) {
        if (money - CalcCost(type, -1, level) < 0) { continue; }
        money -= CalcCost(type, -1, level);
        ret.push_back(TowerInfo(x, y, level, type));
      }
    }
  }
  return ret;
}

void TowerToMask(const Field &field, vector<TowerInfo> towers, int mask[51][51], int money) {
  memset(mask, 0, sizeof(int) * 51 * 51);
  FORIT(it, towers) {
    if (money < it->Money()) { continue; }
    money -= it->Money();
    mask[it->y][it->x] = it->type * 10 + it->level + 11;
  }
}
