if(NOT GMOCK_FOUND)

#find_path(GMOCK_INCLUDE_DIR gmock/gmock.h)
find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
          HINTS $ENV{GMOCK_INSTALL_DIR}/include )

#find_library(GMOCK_LIBRARY NAMES gmock gmock_main )
find_library(GMOCK_LIBRARY NAMES gmock gmock_main
             HINTS $ENV{GMOCK_INSTALL_DIR} )

set(GMOCK_LIBRARIES ${GMOCK_LIBRARY} )
set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(gmock  DEFAULT_MSG
                                  GMOCK_LIBRARY GMOCK_INCLUDE_DIR)

mark_as_advanced(GMOCK_INCLUDE_DIR GMOCK_LIBRARY )

endif()