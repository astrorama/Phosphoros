CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)


#---------------------------------------------------------------
# Load macros and functions for Elements-based projects
find_package(ElementsProject)
#---------------------------------------------------------------

set(CXX_SUGGEST_OVERRIDE ON
    CACHE STRING "Enable the -Wsuggest-override warning"
    FORCE)

# Declare project name and version

elements_project(Phosphoros 1.6.4 USE PhosphorosCore 1.7.0)

