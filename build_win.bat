rmdir build /s /q
mkdir build
cd build
conan install ..
conan build ..
cd ..