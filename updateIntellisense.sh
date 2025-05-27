# use in case that clangd starts glitching

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .

rm -r "CMakeFiles"
rm "cmake_install.cmake"
rm "CMakeCache.txt"
rm "CTestTestfile.cmake"
rm "DartConfiguration.tcl"
rm "Makefile"
rm tests-*