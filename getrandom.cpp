#if defined(_WIN32) || defined(_WIN64)
#include "os_specific/win/getrandom.cpp"
#else
#include "os_specific/nix/getrandom.cpp"
#endif
