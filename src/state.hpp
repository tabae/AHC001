#ifndef __STATE_HPP__
#define __STATE_HPP__

#include "common.hpp"
#include "ryuka.hpp"
#include <numeric>
#include <algorithm>

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
    constexpr int da[4] = {-1, 0, 0, 0};
    constexpr int db[4] = {0, -1, 0, 0};
    constexpr int dc[4] = {0, 0, 1, 0};
    constexpr int dd[4] = {0, 0, 0, 1};
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
                    if(new_rect.size() < in.r[i] && !common::is_overlap(new_rect, res.pos, i)) {
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
