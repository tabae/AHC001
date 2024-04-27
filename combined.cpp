#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
using ll = long long;

#define L 10000

struct Input {
    ll n;
    vector<ll> x, y, r;
    void read();
};

void Input::read() {
    cin >> n;
    x.resize(n);
    y.resize(n);
    r.resize(n);
    for(int i = 0; i < n; i++) {
        cin >> x[i] >> y[i] >> r[i];
    }
}

struct Rect {
    ll a, b, c, d;
    Rect() {}
    Rect(ll _a, ll _b, ll _c, ll _d) : a(_a), b(_b), c(_c), d(_d) {} 
    inline ll size() const;
};

namespace common {
    inline bool in_base(const int, const Rect&);
    inline ll calc_score(const vector<Rect>&);
    inline bool is_overlap(const Rect&, const Rect&);
    inline bool is_overlap(const Rect&, const vector<Rect>&, const int);
    inline bool in_field(const Rect&);
    inline bool in_field(const ll, const ll, const ll, const ll);
    void print(const vector<Rect>&);
};

extern Input in;

inline bool operator==(const Rect& lhs, const Rect& rhs) {
    return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c && lhs.d == rhs.d;
}

inline bool operator!=(const Rect& lhs, const Rect& rhs) {
    return !(lhs == rhs);
}

inline ll Rect::size() const {
    const ll res = (c - a) * (d - b);
    return res;
}

inline bool common::in_base(const int i, const Rect& rect) {
    const bool in_x = rect.a <= in.x[i] && in.x[i] < rect.c;
    const bool in_y = rect.b <= in.y[i] && in.y[i] < rect.d;
    return in_x && in_y;
}

inline ll common::calc_score(const vector<Rect>& out) {
    double p_sum = 0;
    constexpr ll ten_9 = 1000000000;
    for(int i = 0; i < in.n; i++) {
        if(is_overlap(out[i], out, i)) {
            cerr << "Error: Overlap occured." << endl; 
            return -1;
        }
        if(in_base(i, out[i])) {
            const ll s = out[i].size();
            const ll r = in.r[i];
            const double d = 1.0 - (double)min(r, s) / (double)max(r, s);
            const double p = 1.0 - d * d;
            p_sum += p / in.n;
        }
    }
    return round(p_sum * ten_9);
}

inline bool common::is_overlap(const Rect& x, const Rect& y) {
    const ll a = max(x.a, y.a);
    const ll b = max(x.b, y.b);
    const ll c = min(x.c, y.c);
    const ll d = min(x.d, y.d);
    return a < c && b < d;
}

inline bool common::is_overlap(const Rect& x, const vector<Rect>& v, const int skip = -1) {
    for(int i = 0; i < v.size(); i++) {
        const Rect& y = v[i];
        if(i != skip) {
            if(common::is_overlap(x, y)) {
                return true;
            }
        }
    }
    return false;
}


inline bool common::in_field(const Rect& rect) {
    return in_field(rect.a, rect.b, rect.c, rect.d);
}

inline bool common::in_field(const ll a, const ll b, const ll c, const ll d) {
    const bool res = a >= 0 && b >= 0 && c < L && d < L && a < c && b < d;
    return res;
} 

void common::print(const vector<Rect>& rect) {
    for(int i = 0; i < in.n; i++) {
        cout << rect[i].a << " " << rect[i].b << " " << rect[i].c << " " << rect[i].d << endl;
    }
} 

#endif
#ifndef __STATE_HPP__
#define __STATE_HPP__

#ifndef __RYUKA_HPP__
#define __RYUKA_HPP__

#include <random>
using namespace std;

struct RandGenerator {

    random_device seed_gen;
    mt19937 engine;
    mt19937_64 engine64;
    static const int pshift = 1000000000;
    
    RandGenerator() : engine(seed_gen()), engine64(seed_gen()) {}
    
    int rand(int mod) {
        return engine() % mod;
    }
    
    long long randll(long long mod) {
        return engine64() % mod;
    } 
    
    bool pjudge(double p) {
        int p_int;
        if(p > 1) p_int = pshift;
        else p_int = p * pshift;
        return rand(pshift) < p_int;
    }

} ryuka;

#endif
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
#include <iostream>
using namespace std;

Input in;

int main() {

    in.read();

    State stat = State::initState();
    common::print(stat.pos);

    cerr << "my score = " << stat.score << endl;
}