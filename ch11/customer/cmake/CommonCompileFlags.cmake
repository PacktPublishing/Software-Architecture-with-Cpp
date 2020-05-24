list(
  APPEND
  BASE_COMPILE_FLAGS
  "$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall;-Wextra;-pedantic;-Werror>"
  "$<$<CXX_COMPILER_ID:MSVC>:/W4;/WX>")
