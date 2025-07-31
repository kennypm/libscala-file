#include <iostream>
#include <cassert>
#include <math.h>

#include "scala_file.hpp"

int main(int, char**) {
    std::ifstream test_scale;
    test_scale.open("scales/meanquar.scl");
    scala::scale loaded_scale = scala::read_scl(test_scale);

    /*
        Test type conversions in scala::degree ratio
        16 different cases to test
            4 pairs of types
                double, rational
                double, double
                rational, double
                rational, ration
            4 operators
                *
                /
                *=
                /=
    */

    scala::real test_real;

    // We'll just be exhaustive and test every degree against every other
    for(size_t i = 0; i < loaded_scale.get_scale_length(); ++i) {
        for(scala::degree deg : loaded_scale.degrees) {
            test_real = loaded_scale.degrees[i].ratio * deg.ratio;
            assert(test_real.get_double() ==
                   loaded_scale.get_ratio(i) * deg.get_ratio());

            test_real = loaded_scale.degrees[i].ratio / deg.ratio;
            assert(test_real.get_double() ==
                   loaded_scale.get_ratio(i) / deg.get_ratio());

            test_real = loaded_scale.degrees[i].ratio;
            test_real *= deg.ratio;
            assert(test_real.get_double() ==
                   loaded_scale.get_ratio(i) * deg.get_ratio());

            test_real = loaded_scale.degrees[i].ratio;
            test_real /= deg.ratio;
            assert(test_real.get_double() ==
                   loaded_scale.get_ratio(i) / deg.get_ratio());
        }
    }

    test_scale.close();
    return 0;
}
