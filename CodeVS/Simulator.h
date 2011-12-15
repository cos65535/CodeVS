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
		deque<int> target;
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
		int Range2() const {
			int ra[3] = { 4, 5, 2 };
			return square(ra[type] + level);
		}
	};
	struct Enemy {
		EnemyInfo info;
		int x;
		int y;
		int life;
		int wait;
		int charge;
		bool alive;
		Enemy(EnemyInfo info) : info(info), x(info.x), y(info.y), life(info.life), wait(info.speed), charge(0), alive(true) {;}
	};
	vector<StageData> stages;
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
			StageData stage;
			ret &= stage.Load(fp);
			stages.push_back(stage);
		}
		fclose(fp);
		return ret;
	}

  //1ステージのシミューレーション
	pair<int, int> OneStageSimulation(int stage, const vector<TowerInfo> &towers) {
    vector<TowerInfo> nothing;
		pair<int, int> ret(0, 0);
		REP(i, stages[stage].infos.size()) {
			pair<int, int> nret;
      if (i == 0) {
        OneMapSimulation(stage, i, towers, nothing);
      } else {
        OneMapSimulation(stage, i, nothing, towers);
      }
			ret.first += nret.first;
			ret.second += nret.second;
		}
		return ret;
	}

  //1レベルのシミュレーション
	pair<int, int> OneMapSimulation(int stage, int level, const vector<TowerInfo> &towerInfos, const vector<TowerInfo> &oldTowerInfos) {
		pair<int, int> ret(0, 0);
		Field field(stages[stage].field, stages[stage].w, stages[stage].h);
    {
      int nret = field.PutTower(oldTowerInfos);
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

		vector<EnemyInfo> enemyInfos = stages[stage].infos[level].enemy;
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
				enemys.push_back(enemyInfos[index]);
				index++;
			}

      //タワーの処理
      FORIT(it1, towers) {
        REP(i, enemys.size()) {
          int d = square(it1->x - enemys[i].x) + square(it1->y - enemys[i].y);
          bool in = enemys[i].alive && d <= it1->Range2();
          FORIT(it2, it1->target) {
            if (*it2 == i) {
              if (!in) {
                it1->target.erase(it2);
              }
              goto next;
            }
          }
          if (in) {
            it1->target.push_back(i);
          }
next:;
        }

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
      int deadCnt = 0;
      FORIT(it, enemys) {
        if (it->life <= 0) {
          if (it->alive && stage < 40) {
            int money = (it->info.life - (frame - it->info.t)) / 10;
            ret.second += money;
          }
          deadCnt++;
          it->alive = false;
        } else if (field.field[it->y][it->x] == 'g') {
          ret.first++;
          it->life = -1;
          deadCnt++;
          it->alive = false;
        }
      }
      if (deadCnt == enemyInfos.size()) { break; }
		}
		return ret;
	}
};
