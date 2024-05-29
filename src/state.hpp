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

    /*
    アルゴリズム：
        最小の正方形を作成し、ランダムな方向に伸ばせるだけ伸ばしていく。
    */

    constexpr int da[4] = {-1, 0, 0, 0};
    constexpr int db[4] = {0, -1, 0, 0};
    constexpr int dc[4] = {0, 0, 1, 0};
    constexpr int dd[4] = {0, 0, 0, 1};
    
    State res;
    res.pos.resize(in.n);
    
    for(int i = 0; i < in.n; i++) {
        res.pos[i] = Rect(in.x[i], in.y[i], in.x[i]+1, in.y[i]+1);
    }
    
    bool enough_space = true;
    while(enough_space) {
        enough_space = false;
        vector<int> idx(in.n), k_idx(4);
        iota(idx.begin(), idx.end(), 0);
        iota(k_idx.begin(), k_idx.end(), 0);
        shuffle(idx.begin(), idx.end(), ryuka.engine);
        shuffle(k_idx.begin(), k_idx.end(), ryuka.engine);
        for(int i : idx) {
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
    /*
    アルゴリズム：
        ランダムに1つの長方形を選び、その長方形をランダムな方向に単位長伸ばす。
        伸ばした部分がほかの長方形と重なる場合、基点を侵食しないように注意しつつ、ほかの長方形を縮める。
        その後、縮めた長方形については、ほかの長方形と重ならない範囲で、伸ばせる方向に伸ばす。
    */
    
    constexpr int default_delta = 10;

    constexpr int da[4] = {-1, 0, 0, 0};
    constexpr int db[4] = {0, -1, 0, 0};
    constexpr int dc[4] = {0, 0, 1, 0};
    constexpr int dd[4] = {0, 0, 0, 1};
    
    State res = input_state;

    // ランダムに1つの長方形・方向を選ぶ
    const int i = ryuka.rand(in.n);
    const int k = ryuka.rand(4);

    int delta = default_delta;

    // 伸ばした先に、ほかの長方形の基点を侵食してしまう場合、dを縮める
    {
        const int a = res.pos[i].a + da[k] * delta;
        const int b = res.pos[i].b + db[k] * delta;
        const int c = res.pos[i].c + dc[k] * delta;
        const int d = res.pos[i].d + dd[k] * delta;

        for(int j = 0; j < in.n; j++) {
            if(i != j && common::is_overlap(Rect(a, b, c, d), res.pos[j])) {
                //左方向
                if(k == 0) {
                    const int tmp = abs(in.x[j] - res.pos[i].a) - 1;
                    delta = min(delta, tmp);
                }
                //下方向 
                if(k == 1) {
                    const int tmp = abs(in.y[j] - res.pos[i].b) - 1;
                    delta = min(delta, tmp);
                }
                //右方向
                if(k == 2) {
                    const int tmp = abs(in.x[j] - res.pos[i].c) - 1;
                    delta = min(delta, tmp);
                }
                //上方向
                if(k == 3) {
                    const int tmp = abs(in.y[j] - res.pos[i].d) - 1;
                    delta = min(delta, tmp);
                }
            }
        }
    }

    // 伸ばす
    res.pos[i].a = max(0LL, res.pos[i].a + da[k] * delta);
    res.pos[i].b = max(0LL, res.pos[i].b + db[k] * delta);
    res.pos[i].c = min<ll>(L, res.pos[i].c + dc[k] * delta);
    res.pos[i].d = min<ll>(L, res.pos[i].d + dd[k] * delta);

    // 重なっている長方形を列挙
    vector<int> overlapped = common::enumerate_overlaped_rect(res.pos[i], res.pos, i);
    
    // 重なっている長方形を縮める
    for(int j : overlapped) {
        if(k == 0) {
            res.pos[j].c = min(res.pos[j].c, res.pos[i].a);
        }
        if(k == 1) {
            res.pos[j].d = min(res.pos[j].d, res.pos[i].b);
        }
        if(k == 2) {
            res.pos[j].a = max(res.pos[j].a, res.pos[i].c);
        }
        if(k == 3) {
            res.pos[j].b = max(res.pos[j].b, res.pos[i].d);
        }
    }

    // 伸ばせるだけ伸ばす
    {
        bool enough_space = true;
        while(enough_space) {
            enough_space = false;
            vector<int> idx = overlapped;
            vector<int> k_idx(4);
            iota(idx.begin(), idx.end(), 0);
            iota(k_idx.begin(), k_idx.end(), 0);
            shuffle(idx.begin(), idx.end(), ryuka.engine);
            shuffle(k_idx.begin(), k_idx.end(), ryuka.engine);
            for(int i : idx) {
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
    }

    res.calc_score();
    return res;
}

#endif
