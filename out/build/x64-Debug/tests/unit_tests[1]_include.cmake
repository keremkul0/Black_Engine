if(EXISTS "C:/Users/MONSTER/Desktop/bitirme/Black_Engine/out/build/x64-Debug/tests/unit_tests[1]_tests.cmake")
  include("C:/Users/MONSTER/Desktop/bitirme/Black_Engine/out/build/x64-Debug/tests/unit_tests[1]_tests.cmake")
else()
  add_test(unit_tests_NOT_BUILT unit_tests_NOT_BUILT)
endif()
