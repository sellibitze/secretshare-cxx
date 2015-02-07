#if defined(_WIN32) || defined(_WIN64)
#include "os_specific/win/io.cpp"
#else
#include "os_specific/nix/io.cpp"
#endif
