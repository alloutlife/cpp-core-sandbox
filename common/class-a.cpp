#include "class-a.h"

namespace cpp_core_sandbox {

int A::unique_seq_no_{ 1 };
int A::throw_in_ctor_for_seq_no_{ -2 };         // note: `-1` is for moved instance with its undefined state

}
