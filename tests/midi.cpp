#include <iostream>
#include "scala_file.hpp"

using boost::rational;

template <class... Ts>
struct overload : Ts... { using Ts::operator() ...; };

// this is a crude test, just to observe the behavior of the
// rational vs. irrational type conversions
int main(int, char**) {
  std::ifstream test_scl, test_kbm;
    test_scl.open("scales/meanquar.scl");
    test_kbm.open("kbm/12-tet.kbm");
  //  test_scl.open("scales/ptolemy.scl");
  //  test_kbm.open("kbm/white.kbm");
  scala::scale loaded_scl = scala::read_scl(test_scl);
  scala::kbm loaded_kbm = scala::read_kbm(test_kbm);

  for(int i=0; i<128; i++) {
    try {
      scala::degree d = scala::convert_midi(i, loaded_scl, loaded_kbm);
      std::visit(overload{
	  [](auto& val) { std::cout << val << std::endl; }
	}, d.ratio);
    } catch (const std::runtime_error& e) {
      //      std::cout << e.what() << std::endl;
    }
  }
    
  return 0;
}
