#include "common.hpp"
#include "state.hpp"
#include <iostream>
using namespace std;

Input in;

int main() {

    in.read();

    State stat = State::initState();
    common::print(stat.pos);

    cerr << "my score = " << stat.score << endl;
}
