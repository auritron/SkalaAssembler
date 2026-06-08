#include "panic.hpp"

void panic::panic() {
    std::cerr << 
    "An unexpected error has occured in the program, due to a bug causing unintended control flow. Please check your code." 
    << std::endl;
    std::abort();
}