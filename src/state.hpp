#ifndef __STATE_HPP__
#define __STATE_HPP__

#include "common.hpp"
#include "ryuka.hpp"
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace std;

extern Input in;
extern RandGenerator ryuka;

struct State {
    static constexpr long long inf = 1LL<<60;
    long long score;
    vector<Rect> pos;
    State() : score(-inf) {};
    long long calc_score();
    static State initState();
    static State generateState(const State& input_state);
};

long long State::calc_score() {
    score = common::calc_score(pos);
    return score;
}

State State::initState() {
    State res;
    res.pos.resize(in.n);
    for(int i = 0; i < in.n; i++) {
        res.pos[i] = Rect(in.x[i], in.y[i], in.x[i]+1, in.y[i]+1);
    }
    bool enough_space = true;
    constexpr int da[4] = {0, 0, 0, -1};
    constexpr int db[4] = {0, -1, 0, 0};
    constexpr int dc[4] = {0, 0, 1, 0};
    constexpr int dd[4] = {1, 0, 0, 0};
    while(enough_space) {
        enough_space = false;
        vector<int> idx(in.n), k_idx(4);
        iota(idx.begin(), idx.end(), 0);
        iota(k_idx.begin(), k_idx.end(), 0);
        shuffle(idx.begin(), idx.end(), ryuka.engine);
        //shuffle(k_idx.begin(), k_idx.end(), ryuka.engine);
        for(int i : idx) {
            sort(k_idx.begin(), k_idx.end(), [&](int l, int r){
                
                // 最も近い点がなるべく遠い方向に伸ばす
                // 上下左右を分類する必要がある。
                // 上方向： y >= d かつ、y >= -x + d + a かつ、y >= x + d - c
                // 下方向： y <= b かつ、y <= -x + b + c かつ、y <= x + b - a
                // 右方向： x >= c かつ、y <=  x + d - c かつ、y >= -x + b + c
                // 左方向： x <= a かつ、y <= -x + d + a かつ、y >= x + b - a                
                auto get_dir = [&](int a, int b, int c, int d, int x, int y) -> int {
                    if(y >= d && y >= -x + d + a && y >= x + d - c) return 0;
                    if(y <= b && y <= -x + b + c && y <= x + b - a) return 1;
                    if(x >= c && y <= x + d - c && y >= -x + b + c) return 2;
                    if(x <= a && y <= -x + d + a && y >= x + b - a) return 3;
                    assert(false);
                };

                // 距離の定義：
                // 上方向： a <= x <= c ならば、y - d, x < a ならば、y - d + a - x, x > c ならば、y - d + x - c
                // 下方向： a <= x <= c ならば、b - y, x < a ならば、b - y + a - x, x > c ならば、b - y + x - c
                // 右方向： b <= y <= d ならば、x - c, y < b ならば、x - c + b - y, y > d ならば、x - c + y - d
                // 左方向： b <= y <= d ならば、a - x, y < b ならば、a - x + b - y, y > d ならば、a - x + y - d
                auto get_dist = [&](int a, int b, int c, int d, int x, int y) -> ll {
                    const int dir = get_dir(a, b, c, d, x, y);
                    if(dir == 0) {
                        if(a <= x && x <= c) return y - d;
                        if(x < a) return y - d + a - x;
                        if(x > c) return y - d + x - c;
                    }
                    if(dir == 1) {
                        if(a <= x && x <= c) return b - y;
                        if(x < a) return b - y + a - x;
                        if(x > c) return b - y + x - c;
                    }
                    if(dir == 2) {
                        if(b <= y && y <= d) return x - c;
                        if(y < b) return x - c + b - y;
                        if(y > d) return x - c + y - d;
                    }
                    if(dir == 3) {
                        if(b <= y && y <= d) return a - x;
                        if(y < b) return a - x + b - y;
                        if(y > d) return a - x + y - d;
                    }
                    assert(false);
                };

                auto calc_nearest_dist = [&](int i, int dir) -> ll {
                    int nearest = -1;
                    ll min_dist = inf;
                    for(int j = 0; j < in.n; j++) {
                        if(i == j) continue;
                        if(get_dir(res.pos[i].a, res.pos[i].b, res.pos[i].c, res.pos[i].d, in.x[j], in.y[j]) != dir) continue;
                        const ll dist = get_dist(res.pos[i].a, res.pos[i].b, res.pos[i].c, res.pos[i].d, in.x[j], in.y[j]);
                        if(dist < min_dist) {
                            min_dist = dist;
                            nearest = j;
                        }
                    }
                    return min_dist;
                };

                return calc_nearest_dist(i, l) > calc_nearest_dist(i, r);
            });

            for(int k : k_idx) {
                const ll a = res.pos[i].a + da[k];
                const ll b = res.pos[i].b + db[k];
                const ll c = res.pos[i].c + dc[k];
                const ll d = res.pos[i].d + dd[k];
                if(common::in_field(a, b, c, d)) {
                    const Rect new_rect = Rect(a, b, c, d);
                    if(common::calc_p(i, res.pos[i]) < common::calc_p(i, new_rect) && !common::is_overlap(new_rect, res.pos, i)) {
                        res.pos[i] = new_rect;
                        enough_space = true;
                        break;
                    }
                }
            }
        }
    }
    res.calc_score();
    return res;
}

State State::generateState(const State& input_state) {
    State res = input_state;
    res.calc_score();
    return res;
}

#endif
