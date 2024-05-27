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

#endif