#pragma once

#include "Base.h"
#include "Field.h"
#include "Structure.h"

struct Simulator {
  struct Tower {
    int x;
    int y;
    int level;
    int type;
    int charge;
    vector<int> target;
    Tower(TowerInfo info) : x(info.x), y(info.y), level(info.level), type(info.type), charge(0) {;}
    int Attack() const {
      int at[3] = { 10, 20, 3 };
      return at[type] * (level + 1);
    }
    int Wait() const {
      if (type == 2) { return 20; }
      int wa[2] = { 10, 20 };
      return wa[type] - 2 * level;
    }
    inline int Range2() const {
      int ra[3] = { 4, 5, 2 };
      return square(ra[type] + level);
    }
  };
  struct Enemy {
    EnemyInfo info;
    int index;
    int x;
    int y;
    int life;
    int wait;
    int charge;
    bool alive;
    Enemy(EnemyInfo info, int index) : info(info), index(index), x(info.x), y(info.y), life(info.life), wait(info.speed), charge(0), alive(true) {;}
  };

  vector<MapInfo> stages;
  //static int totalTime;
  Simulator() {;}
  Simulator(const char *filename) { Load(filename); }
  void Release() {
    stages.clear();
  }
  bool Load(const char *filename) {
    Release();
    bool ret = true;
    FILE *fp = fopen(filename, "r");
    assert(fp != NULL);
    int stageCnt = -1;
    int v = fscanf(fp, "%d", &stageCnt);
    assert(v == 1);
    REP(i, stageCnt) {
      MapInfo stage;
      ret &= stage.Load(fp);
      stages.push_back(stage);
    }
    fclose(fp);
    return ret;
  }

  //void SkipInput() {
  //  while (scanf("%*s") != EOF);
  //}

  //void PrintMap(int map) {
  //  printf("1\n");
  //  stages[map].Print();
  //}

  static pair<int, int> MapSimulation(bool earn, const MapInfo &mapInfo, int stage, const vector<TowerInfo> &towers, int damage = 10) {
    damage = min(damage, 10);
    Simulator simulator;
    stage = 0;
    simulator.stages.push_back(mapInfo);
    return simulator.MapSimulation(earn, stage, towers);
  }
  //static pair<int, int> LevelSimulation(const MapInfo &mapInfo, int stage, int level, const vector<TowerInfo> &towers) {
  //  Simulator simulator;
  //  if (stage >= 40) {
  //    simulator.stages.resize(40);
  //    stage = 40;
  //  } else {
  //    stage = 0;
  //  }
  //  simulator.stages.push_back(mapInfo);
  //  return simulator.LevelSimulation(stage, level, towers);
  //}

  //1マップのシミューレーション
  pair<int, int> MapSimulation(bool earn, int stage, const vector<TowerInfo> &towers, int damage = 10) {
    damage = min(damage, 10);
    vector<TowerInfo> nothing;
    pair<int, int> ret(0, 0);
    REP(i, stages[stage].levels.size()) {
      pair<int, int> nret;
      if (i == 0) {
        nret = LevelSimulation(earn, stage, i, towers, damage - nret.first);
      } else {
        nret = LevelSimulation(earn, stage, i, nothing, damage - nret.first);
      }
      ret.first += nret.first;
      ret.second += nret.second;
      if (ret.first >= damage) {
        ret.first = 10;
        break;
      }
    }
    return ret;
  }

  //1レベルのシミュレーション
  pair<int, int> LevelSimulation(bool earn, int stage, int level, const vector<TowerInfo> &towerInfos, int damage = 10) {
    damage = min(damage, 10);
    //int start = timeGetTime();
    pair<int, int> ret(0, 0);
    Field field(stages[stage].field, stages[stage].w, stages[stage].h);
    {
      int nret = field.PutTower(stages[stage].levels[level].tower);
      if (nret == -1) {
        ret.first = -1000;
        return ret;
      }
      nret = field.PutTower(towerInfos);
      if (nret == -1) {
        ret.first = -1000;
        return ret;
      }
      ret.second -= nret;
    }
    //REP(y, field.h) {
    //  REP(x, field.w) {
    //    if (field.move[y][x] >= 9) { printf("#"); }
    //    else {
    //      printf("%d", field.move[y][x]);
    //    }
    //  }
    //    puts("");
    //}

    vector<EnemyInfo> enemyInfos = stages[stage].levels[level].enemy;
    stable_sort(enemyInfos.begin(), enemyInfos.end());
    vector<Enemy> enemys;
    vector<Tower> towers;
    REP(y, field.h) {
      REP(x, field.w) {
        int type = field.field[y][x] / 1000 - 1;
        int level = field.field[y][x] % 1000 / 100 - 1;
        if (type != -1) {
          towers.push_back(Tower(TowerInfo(x, y, level, type)));
        }
      }
    }

    int index = 0;
    int deadCnt = 0;
    for (int frame = 0; frame < (1 << 30); frame++) {
      //移動させる
      const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
      const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
      FORIT(it, enemys) {
        if (!it->alive) { continue; }
        it->charge++;
        int dir = field.move[it->y][it->x];
        int need = it->wait * (dir % 2 == 0 ? 10 : 14) / 10;
        if (need == it->charge) {
          it->x += dx[dir];
          it->y += dy[dir];
          it->charge = 0;
        }
      }

      //敵を追加
      while (index < (int)enemyInfos.size() && enemyInfos[index].t == frame) {
        enemys.push_back(Enemy(enemyInfos[index], index));
        index++;
      }

      //タワーの処理
      FORIT(it2, enemys) {
        if (!it2->alive || it2->charge != 0) { continue; }
        FORIT(it1, towers) {
          int d = square(it1->x - it2->x) + square(it1->y - it2->y);
          bool in = d <= it1->Range2();
          FORIT(it3, it1->target) {
            if (*it3 == it2->index) {
              if (!in) {
                it1->target.erase(it3);
              }
              goto next;
            }
          }
          if (in) {
            it1->target.push_back(it2->index);
          }
next:;
        }
      }

      FORIT(it1, towers) {
        //弾を撃てるなら撃つ
        if (it1->charge < 0 && !it1->target.empty()) {
          int target = it1->target.front();
          enemys[target].life -= it1->Attack();
          if (it1->type == 2) {
            enemys[target].charge -= 2;
          }
          it1->charge = it1->Wait();
        }
        it1->charge--;
      }

      //敵が死亡してるかどうかとゴールにたどり着いてるかどうかを確認
      FORIT(it1, enemys) {
        if (!it1->alive) { continue; }
        if (it1->life <= 0) {
          if (earn) {
            int money = (it1->info.life - (frame - it1->info.t)) / 10;
            ret.second += money;
          }
          deadCnt++;
          it1->alive = false;
        } else if (field.field[it1->y][it1->x] == 'g') {
          ret.first++;
          it1->life = -1;
          deadCnt++;
          it1->alive = false;
          if (ret.first >= damage) { goto end; }
        }
        if (!it1->alive) {
          FORIT(it2, towers) {
            FORIT(it3, it2->target) {
              if (*it3 == it1->index) {
                it2->target.erase(it3);
                break;
              }
            }
          }
        }
      }
      if (deadCnt == (int)enemyInfos.size()) {
        break;
      }
    }
end:

    int nstage = stage;
    int nlevel = level + 1;
    if (nlevel == 25) { nstage++; nlevel = 0; }
    if ((int)stages.size() > nstage && (int)stages[nstage].levels.size() > nlevel) {
      if (nlevel != 0) {
        stages[nstage].levels[nlevel].tower.clear();
        FORIT(it, stages[stage].levels[level].tower) {
          stages[nstage].levels[nlevel].tower.push_back(*it);
        }
        FORIT(it, towerInfos) {
          stages[nstage].levels[nlevel].tower.push_back(*it);
        }
      }
      stages[nstage].levels[nlevel].life = stages[stage].levels[level].life - ret.first;
      stages[nstage].levels[nlevel].money = stages[stage].levels[level].money + ret.second;
    }
    //int end = timeGetTime();
    //totalTime += end - start;
    return ret;
  }



    //厳密ではない1レベルのシミュレーション
    pair<int, int> LevelSimulationAproximate(bool earn, int stage, int level, const vector<TowerInfo> &towerInfos, int damage = 10) {
    damage = min(damage, 10);
    //int start = timeGetTime();
    pair<int, int> ret(0, 0);
    Field field(stages[stage].field, stages[stage].w, stages[stage].h);
    {
      int nret = field.PutTower(stages[stage].levels[level].tower);
      if (nret == -1) {
        ret.first = -1000;
        return ret;
      }
      nret = field.PutTower(towerInfos);
      if (nret == -1) {
        ret.first = -1000;
        return ret;
      }
      ret.second -= nret;
    }

    vector<EnemyInfo> enemyInfos = stages[stage].levels[level].enemy;
    stable_sort(enemyInfos.begin(), enemyInfos.end());
    vector<Enemy> enemys;
    vector<Tower> towers;
    REP(y, field.h) {
      REP(x, field.w) {
        int type = field.field[y][x] / 1000 - 1;
        int level = field.field[y][x] % 1000 / 100 - 1;
        if (type != -1) {
          towers.push_back(Tower(TowerInfo(x, y, level, type)));
        }
      }
    }

    int index = 0;
    int deadCnt = 0;
    for (int frame = 0; frame < (1 << 30); frame++) {
      //移動させる
      const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
      const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
      FORIT(it, enemys) {
        if (!it->alive) { continue; }
        it->charge++;
        int dir = field.move[it->y][it->x];
        int need = it->wait * (dir % 2 == 0 ? 10 : 14) / 10;
        if (need == it->charge) {
          it->x += dx[dir];
          it->y += dy[dir];
          it->charge = 0;
        }
      }

      //敵を追加
      while (index < (int)enemyInfos.size() && enemyInfos[index].t == frame) {
        enemys.push_back(Enemy(enemyInfos[index], index));
        index++;
      }

      //タワーの処理
      FORIT(it2, enemys) {
        if (!it2->alive || it2->charge != 0) { continue; }
        FORIT(it1, towers) {
          if (it1->charge < 0) {
            int d = square(it1->x - it2->x) + square(it1->y - it2->y);
            bool in = d <= it1->Range2();
            FORIT(it3, it1->target) {
              if (*it3 == it2->index) {
                if (!in) {
                  it1->target.erase(it3);
                }
                goto next;
              }
            }
            if (in) {
              it1->target.push_back(it2->index);
            }
          }
next:;
        }
      }

      FORIT(it1, towers) {
        //弾を撃てるなら撃つ
        if (it1->charge < 0 && !it1->target.empty()) {
          int target = it1->target.front();
          enemys[target].life -= it1->Attack();
          if (it1->type == 2) {
            enemys[target].charge -= 2;
          }
          it1->charge = it1->Wait();
        }
        it1->charge--;
      }

      //敵が死亡してるかどうかとゴールにたどり着いてるかどうかを確認
      FORIT(it1, enemys) {
        if (!it1->alive) { continue; }
        if (it1->life <= 0) {
          if (earn) {
            int money = (it1->info.life - (frame - it1->info.t)) / 10;
            ret.second += money;
          }
          deadCnt++;
          it1->alive = false;
        } else if (field.field[it1->y][it1->x] == 'g') {
          ret.first++;
          it1->life = -1;
          deadCnt++;
          it1->alive = false;
          if (ret.first >= damage) { goto end; }
        }
        if (!it1->alive) {
          FORIT(it2, towers) {
            FORIT(it3, it2->target) {
              if (*it3 == it1->index) {
                it2->target.erase(it3);
                break;
              }
            }
          }
        }
      }
      if (deadCnt == (int)enemyInfos.size()) {
        break;
      }
    }
end:

    int nstage = stage;
    int nlevel = level + 1;
    if (nlevel == 25) { nstage++; nlevel = 0; }
    if ((int)stages.size() > nstage && (int)stages[nstage].levels.size() > nlevel) {
      if (nlevel != 0) {
        stages[nstage].levels[nlevel].tower.clear();
        FORIT(it, stages[stage].levels[level].tower) {
          stages[nstage].levels[nlevel].tower.push_back(*it);
        }
        FORIT(it, towerInfos) {
          stages[nstage].levels[nlevel].tower.push_back(*it);
        }
      }
      stages[nstage].levels[nlevel].life = stages[stage].levels[level].life - ret.first;
      stages[nstage].levels[nlevel].money = stages[stage].levels[level].money + ret.second;
    }
    //int end = timeGetTime();
    //totalTime += end - start;
    return ret;
  }

};
//int Simulator::totalTime = 0;
