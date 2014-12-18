if(NOT GTEST_FOUND)

#  find_path(GTEST_INCLUDE_DIRS gtest )
  find_path(GTEST_INCLUDE_DIRS gtest PATHS $ENV{GTEST_INSTALL_DIR}/include)

#  find_library(GTEST_LIBRARIES NAMES gtest gtest_main )
  find_library(GTEST_LIBRARIES NAMES gtest gtest_main PATHS $ENV{GTEST_INSTALL_DIR})

  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(gtest DEFAULT_MSG GTEST_INCLUDE_DIRS GTEST_LIBRARIES)

  mark_as_advanced(GTEST_FOUND GTEST_INCLUDE_DIRS GTEST_LIBRARIES)

endif()