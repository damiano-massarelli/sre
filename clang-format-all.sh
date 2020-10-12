find src/ test/ -type f -iname "*.cpp" -o -type f -iname "*.h" | while read -r i; do clang-format -i "$i"; done

