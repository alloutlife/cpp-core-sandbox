#include <iostream>
#include <sstream>

int main() {
    std::ostringstream oss;
    oss << "blob";
    std::ostream oss2{oss.rdbuf()}; // share the buffer with a different ostream

    // Change conversion rules in the second stream.
    oss2.setf(std::ios::showpos);
    oss2.width(8);
    oss2 << 9;
    oss << "-";
    oss << 1;

    auto val = oss.str(); // "blob      +9-1"

    return 0;
}