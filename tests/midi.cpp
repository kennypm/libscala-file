#include <iostream>
#include "scala_file.hpp"

// this is a crude test, just to observe the behavior of the
// rational vs. irrational type conversions
int main(int, char**) {
  std::ifstream test_scl, test_kbm;
  test_scl.open("scales/meanquar.scl");
  test_kbm.open("kbm/12-tet.kbm");
  scala::scale loaded_scl = scala::read_scl(test_scl);
  scala::kbm loaded_kbm = scala::read_kbm(test_kbm);
  //  loaded_kbm.reference_note = loaded_kbm.middle_note;
  
  for(int i=0; i<128; i++) {
    try {
      scala::degree d = scala::convert_midi(i, loaded_scl, loaded_kbm);
      std::visit(scala::degree::overload{
	  [](auto& val) { std::cout << val << std::endl; }
	}, d.ratio);
    } catch (const std::runtime_error& e) {
      //      std::cout << e.what() << std::endl;
    }
  }

  return 0;
}
