elements_subdir(PhzLikelihood)

elements_depends_on_subdirs(PhzDataModel PhzOutput)

find_package(gmock)
find_package(gtest)

if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE tests/src/LikelihoodAlgorithm_test.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()

#===== Libraries ===============================================================
elements_add_library(PhzLikelihood src/lib/*.cpp
                  LINK_LIBRARIES PhzDataModel PhzOutput
                  PUBLIC_HEADERS PhzLikelihood )

#===== Boost tests =============================================================
elements_add_unit_test(ScaleFactorFunctor_test tests/src/ScaleFactorFunctor_test.cpp
                       LINK_LIBRARIES PhzLikelihood TYPE Boost)
elements_add_unit_test(ChiSquareFunctor_test tests/src/ChiSquareFunctor_test.cpp
                       LINK_LIBRARIES PhzLikelihood TYPE Boost)

#===== Tests using GMock =======================================================
if(GMOCK_FOUND AND GTEST_FOUND)
  elements_add_unit_test(LikelihoodAlgorithm_test tests/src/LikelihoodAlgorithm_test.cpp
                         LINK_LIBRARIES gmock gtest PhzLikelihood TYPE Boost
                         INCLUDE_DIRS gmock gtest)
  elements_add_unit_test(SourcePhzFunctor_test tests/src/SourcePhzFunctor_test.cpp
                         LINK_LIBRARIES gmock gtest PhzLikelihood TYPE Boost
                         INCLUDE_DIRS gmock gtest)
  elements_add_unit_test(SumMarginalizationFunctor_test tests/src/SumMarginalizationFunctor_test.cpp
                         LINK_LIBRARIES gmock gtest PhzLikelihood TYPE Boost
                         INCLUDE_DIRS gmock gtest)
  elements_add_unit_test(MaxMarginalizationFunctor_test tests/src/MaxMarginalizationFunctor_test.cpp
                         LINK_LIBRARIES gmock gtest PhzLikelihood TYPE Boost
                         INCLUDE_DIRS gmock gtest)
endif(GMOCK_FOUND AND GTEST_FOUND)