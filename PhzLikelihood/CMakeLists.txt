elements_subdir(PhzLikelihood)

elements_depends_on_subdirs(PhzDataModel)

#===== Libraries ===============================================================
elements_add_library(PhzLikelihood src/lib/*.cpp
                  LINK_LIBRARIES PhzDataModel
                  PUBLIC_HEADERS PhzLikelihood )

#===== Boost tests =============================================================

elements_add_unit_test(LikelihoodAlgorithm_test tests/src/LikelihoodAlgorithm_test.cpp
                       LINK_LIBRARIES PhzLikelihood TYPE Boost)