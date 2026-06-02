# FindODE.cmake - Find the Open Dynamics Engine
# Provides: ODE::ODE target if found

find_path(ODE_INCLUDE_DIR
  NAMES ode/ode.h
  PATH_SUFFIXES ode
)

find_library(ODE_LIBRARY
  NAMES ode
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ODE
  FOUND_VAR ODE_FOUND
  REQUIRED_VARS ODE_LIBRARY ODE_INCLUDE_DIR
)

if(ODE_FOUND AND NOT TARGET ODE::ODE)
  add_library(ODE::ODE UNKNOWN IMPORTED)
  set_target_properties(ODE::ODE PROPERTIES
    IMPORTED_LOCATION "${ODE_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${ODE_INCLUDE_DIR}"
  )
  # ODE requires C++ linker
  set_target_properties(ODE::ODE PROPERTIES
    INTERFACE_LINK_LIBRARIES "stdc++"
  )
endif()

mark_as_advanced(ODE_INCLUDE_DIR ODE_LIBRARY)
