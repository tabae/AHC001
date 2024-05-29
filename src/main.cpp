#include "common.hpp"
#include "state.hpp"
#include "annealer.hpp"
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
