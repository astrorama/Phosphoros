include(ElementsUtils)

include_guard()

if(APPLE)

  if("$ENV{QTDIR}" STREQUAL "")
    if(NOT "$ENV{MACPORT_LOCATION}" STREQUAL "")
      message(STATUS "Using MacPort location ($ENV{MACPORT_LOCATION}) to locate QTDIR")
      set(qt4_loc "$ENV{MACPORT_LOCATION}/libexec/qt4")
    else()
      message(STATUS "Falling back on /opt/local to locate QTDIR") 
      set(qt4_loc "/opt/local/libexec/qt4")
    endif()
    if(IS_DIRECTORY ${qt4_loc})
      set(ENV{QTDIR} "${qt4_loc}")
    endif()
 
  else()
     message(STATUS "QTDIR is already defined to $ENV{QTDIR}")
  endif()

endif()



