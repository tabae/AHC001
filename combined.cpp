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
    inline double calc_p(const int, const Rect&);
    inline ll calc_score(const vector<Rect>&);
    inline double calc_area_usage(const vector<Rect>&);
    inline bool is_overlap(const Rect&, const Rect&);
    inline bool is_overlap(const Rect&, const vector<Rect>&, const int);
    inline bool in_field(const Rect&);
    inline bool in_field(const ll, const ll, const ll, const ll);
    void print(const vector<Rect>&);
    vector<int> enumerate_overlaped_rect(const Rect&, const vector<Rect>&, const int);
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

inline double common::calc_p(const int i, const Rect& rect) {
    const double s = rect.size();
    const double r = in.r[i];
    const double tmp = 1 - min(r, s) / max(r, s);
    const double res = 1 - tmp * tmp;
    return res;
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

inline double common::calc_area_usage(const vector<Rect>& out) {
    const double sum = L * L;
    double used = 0;
    for(int i = 0; i < in.n; i++) {
        used += out[i].size();
    }
    const double usage = used / sum;
    return usage;
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

vector<int> common::enumerate_overlaped_rect(const Rect& r, const vector<Rect>& v, const int skip = -1) {
    vector<int> res;
    for(int j = 0; j < v.size(); j++) {
        if(j != skip && is_overlap(r, v[j])) {
            res.push_back(j);
        }
    }
    return res;
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

    const int i = ryuka.rand(in.n);
    const int k = ryuka.rand(4);

    int delta = default_delta;

    {
        const int a = res.pos[i].a + da[k] * delta;
        const int b = res.pos[i].b + db[k] * delta;
        const int c = res.pos[i].c + dc[k] * delta;
        const int d = res.pos[i].d + dd[k] * delta;

        for(int j = 0; j < in.n; j++) {
            if(i != j && common::is_overlap(Rect(a, b, c, d), res.pos[j])) {
                if(k == 0) {
                    const int tmp = abs(in.x[j] - res.pos[i].a) - 1;
                    delta = min(delta, tmp);
                }
                if(k == 1) {
                    const int tmp = abs(in.y[j] - res.pos[i].b) - 1;
                    delta = min(delta, tmp);
                }
                if(k == 2) {
                    const int tmp = abs(in.x[j] - res.pos[i].c) - 1;
                    delta = min(delta, tmp);
                }
                if(k == 3) {
                    const int tmp = abs(in.y[j] - res.pos[i].d) - 1;
                    delta = min(delta, tmp);
                }
            }
        }
    }

    res.pos[i].a = max(0LL, res.pos[i].a + da[k] * delta);
    res.pos[i].b = max(0LL, res.pos[i].b + db[k] * delta);
    res.pos[i].c = min<ll>(L, res.pos[i].c + dc[k] * delta);
    res.pos[i].d = min<ll>(L, res.pos[i].d + dd[k] * delta);

    vector<int> overlapped = common::enumerate_overlaped_rect(res.pos[i], res.pos, i);
    
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
#ifndef __ANNEALER_HPP__
#define __ANNEALER_HPP__

#ifndef __TOKI_HPP__
#define __TOKI_HPP__

#include <sys/time.h>
#include <cstddef>

struct Timer {

    double global_start;
    
    double gettime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec * 1e-6;
    }
    
    void init() {
        global_start = gettime();
    }
    
    double elapsed() {
        return gettime() - global_start;
    }
} toki;

#endif

extern Timer toki;
extern RandGenerator ryuka;

template<class STATE>
struct IterationControl {
    long long iteration_counter;
    long long swap_counter;
    double average_time;
    double start_time;
    IterationControl() : iteration_counter(0), swap_counter(0) {}
    STATE climb(double time_limit, STATE initial_state);
    STATE anneal(double time_limit, double temp_start, double temp_end, STATE initial_state);
};

template<class STATE>
STATE IterationControl<STATE>::climb(double time_limit, STATE initial_state) {
    start_time = toki.gettime();
    average_time = 0;
    STATE best_state = initial_state;
    double time_stamp = start_time;
    cerr << "Starts climbing...\n";
    while(time_stamp - start_time + average_time < time_limit) {
        STATE current_state = STATE::generateState(best_state);
        if(current_state.score > best_state.score) {
            swap(best_state, current_state);
            swap_counter++;
        }
        iteration_counter++;
        time_stamp = toki.gettime();
        average_time = (time_stamp - start_time) / iteration_counter;
    }
    cerr << "Iterated " << iteration_counter << " times and swapped " << swap_counter << " times.\n";
    return best_state;
}

template<class STATE>
STATE IterationControl<STATE>::anneal(double time_limit, double temp_start, double temp_end, STATE initial_state) {
    start_time = toki.gettime();
    average_time = 0;
    STATE best_state = initial_state;
    double elapsed_time = 0;
    cerr << "Starts annealing...\n";
    while(elapsed_time + average_time < time_limit) {
        double normalized_time = elapsed_time / time_limit;
        double temp_current = pow(temp_start, 1.0 - normalized_time) * pow(temp_end, normalized_time);
        STATE current_state = STATE::generateState(best_state);
        long long delta = current_state.score - best_state.score;
        if(delta > 0 || ryuka.pjudge(exp(1.0 * delta / temp_current)) ) {
            swap(best_state, current_state);
            swap_counter++;
        }
        iteration_counter++;
        elapsed_time = toki.gettime() - start_time;
        average_time = elapsed_time / iteration_counter;
    }
    cerr << "Iterated " << iteration_counter << " times and swapped " << swap_counter << " times.\n";
    return best_state;
}

#endif
#include <iostream>
using namespace std;

Input in;

int main() {

    in.read();

    IterationControl<State> sera;
    State stat = sera.climb(1.8, State::initState());
    common::print(stat.pos);

    cerr << "my score = " << stat.score << endl;
    cerr << "area usage = " << common::calc_area_usage(stat.pos) << endl;
}