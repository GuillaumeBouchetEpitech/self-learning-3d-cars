
find ./src/framework -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
find ./tests/src -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
