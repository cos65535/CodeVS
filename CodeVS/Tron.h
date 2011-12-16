#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"


namespace Tron {
  int Put(const Field &field, int mask[51][51], int x, int y, int &use) {
    if (field.field[y][x] != '0') { return use; }
    if (mask[y][x] != 0) { return use; }
    mask[y][x] = 1;
    use++;
    return use;
  }

  vector<TowerInfo> TronAI(const MapInfo &mapInfo, const int map, const int level) {
    if (level != 0) { return vector<TowerInfo>(); }
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };
    const int h = mapInfo.h;
    const int w = mapInfo.w;
    Field field(mapInfo.field, mapInfo.w, mapInfo.h);
    int bestMask[51][51];
    int bestDist = -1;
      MEMSET(bestMask, 0x0f);
    REP(iter, 1000) {
      int mask[51][51];
      int dist = 0;
      int use = 0;
      int target = rand() % field.gs.size();
      MEMSET(mask, 0);
      //target以外を埋める
      REP(i, field.gs.size()) {
        if (i == target) { continue; }
        REP(dir, 4) {
          int nx = field.gs[i].x + dx[dir];
          int ny = field.gs[i].y + dy[dir];
          Put(field, mask, nx, ny, use);
        }
      }
      if (field.CalcDist(mask) == -1) { iter--; continue; }

      int px = field.gs[target].x;
      int py = field.gs[target].y;
      int ppx = px;
      int ppy = py;
      int dir = rand() % 4;
      REP(iter, 1000) {
        if (use > 200) { break; }
        dir = (dir + 3) % 4;;
        if (rand() % 100 < 10) {
          dir = rand() % 4;
        }
        int nx = px + dx[dir];
        int ny = py + dy[dir];
        if (field.field[ny][nx] != '0' || mask[ny][nx] != 0) { continue; }
        if (nx == ppx && ny == ppy) { continue; }
        bool ng = false;
        REP(d, 4) {
          int nnx = px + dx[d];
          int nny = py + dy[d];
          if (field.field[nny][nnx] != '0' || mask[nny][nnx] != 0) { continue; }
          if (nnx == ppx && nny == ppy) { continue; }
          if (dir == d) { continue; }
          ng |= !field.OK(mask, nnx, nny);
          Put(field, mask, nnx, nny, use);
        }
        if (ng) {
          REP(d, 4) {
            int nnx = px + dx[d];
            int nny = py + dy[d];
            if (mask[nny][nnx]) {
              use--;
              mask[nny][nnx] = 0;
            }
          }
        }
        ppx = px;
        ppy = py;
        px = nx;
        py = ny;
      }

    dist = field.CalcDist(mask);
    printf("%d %d\n", dist, use);
    REP(y, h) {
      REP(x, w) {
        if (field.field[y][x] != '0') {
          printf("%c", field.field[y][x]);
        } else if (mask[y][x] != 0) {
          printf("2");
        } else {
          printf(".");
        }
      }
      puts(""); 
    }
      // bestを更新
        if (dist > bestDist) {
        memcpy(bestMask, mask, sizeof(bestMask));
        bestDist = dist;
      }
    }
    printf("%d\n", bestDist);
    REP(y, h) {
      REP(x, w) {
        if (field.field[y][x] != '0') {
          printf("%c", field.field[y][x]);
        } else if (bestMask[y][x] != 0) {
          printf("2");
        } else {
          printf(".");
        }
      }
      puts("");
    } 

    // TODO どこに強いタワーを置くかを適切に求める
    vector<TowerInfo> ret;
    REP(y, h) {
      REP(x, w) {
        if (bestMask[y][x]) {
          ret.push_back(TowerInfo(x, y, 4, 0));
        }
      }
    }
    return ret;
  }
}