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

    scala::degree test_degree(1,1);

    // We'll just be exhaustive and test every degree against every other
    for(int scale_index = 0; scale_index < loaded_scale.degrees.size(); scale_index++) {
        for(scala::degree deg : loaded_scale.degrees) {
            test_degree = loaded_scale.degrees[scale_index] * deg;
            assert(test_degree.get_double() == loaded_scale.get_ratio(scale_index) * deg.get_ratio());
            test_degree = loaded_scale.degrees[scale_index] / deg;
            assert(test_degree.get_double() == loaded_scale.get_ratio(scale_index) / deg.get_ratio());
            test_degree = loaded_scale.degrees[scale_index];
            test_degree *= deg;
            assert(test_degree.get_double() == loaded_scale.get_ratio(scale_index) * deg.get_ratio());
            test_degree = loaded_scale.degrees[scale_index];
            test_degree /= deg;
            assert(test_degree.get_double() == loaded_scale.get_ratio(scale_index) / deg.get_ratio());
        }
    }

    test_scale.close();
    return 0;
}
