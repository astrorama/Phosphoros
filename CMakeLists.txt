CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)


#---------------------------------------------------------------
# Load macros and functions for Elements-based projects
find_package(ElementsProject)
#---------------------------------------------------------------

set(CXX_SUGGEST_OVERRIDE ON
    CACHE STRING "Enable the -Wsuggest-override warning"
    FORCE)

# Declare project name and version

elements_project(Phosphoros 2.0 USE PhosphorosCore 2.0.0)

