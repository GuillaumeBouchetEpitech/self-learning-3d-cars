
find ./src/ -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
