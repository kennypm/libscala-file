 #include <iostream>
#include <cassert>
#include <math.h>

#include "scala_file.hpp"

int main(int, char**) {
    std::ifstream test_scale;
    test_scale.open("scales/just.scl");
    scala::scale loaded_scale = scala::read_scl(test_scale);

    /*
        Test type conversions in scala::degree ratio
        Make sure rationals are preserved as expected
    */

    scala::degree test_degree(1,1);

    // We'll just be exhaustive and test every degree against every other
    for(int index = 0; index < loaded_scale.degrees.size(); index++) {
        for(scala::degree deg : loaded_scale.degrees) {
            test_degree = loaded_scale.degrees[index] * deg;
            assert(test_degree.is_rational());
            assert(test_degree.get_rational() ==
                   loaded_scale.degrees[index].get_rational() * deg.get_rational());

            test_degree = loaded_scale.degrees[index] / deg;
            assert(test_degree.is_rational());
            assert(test_degree.get_rational() ==
                   loaded_scale.degrees[index].get_rational() / deg.get_rational());

            test_degree = loaded_scale.degrees[index];
            test_degree *= deg;
            assert(test_degree.is_rational());
            assert(test_degree.get_rational() ==
                   loaded_scale.degrees[index].get_rational() * deg.get_rational());

            test_degree = loaded_scale.degrees[index];
            test_degree /= deg;
            assert(test_degree.is_rational());
            assert(test_degree.get_rational() ==
                   loaded_scale.degrees[index].get_rational() / deg.get_rational());
        }
    }

    test_scale.close();
    return 0;
}
