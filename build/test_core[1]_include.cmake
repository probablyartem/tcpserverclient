if(EXISTS "/home/artem/Documents/cpp_test_make/templateofclientservermatching/build/test_core[1]_tests.cmake")
  include("/home/artem/Documents/cpp_test_make/templateofclientservermatching/build/test_core[1]_tests.cmake")
else()
  add_test(test_core_NOT_BUILT test_core_NOT_BUILT)
endif()