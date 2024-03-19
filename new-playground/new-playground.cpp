#include <string>

int main(void) {

    char *buf = new char[sizeof(std::string)];

    // Placement new, uses memory buffer provided in input parameter
    std::string *s = new (buf) std::string();
    s->resize(1000);
    return 0;
}