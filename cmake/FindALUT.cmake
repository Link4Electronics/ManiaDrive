# FindALUT.cmake - Find the OpenAL Utility Toolkit (ALUT)

find_path(ALUT_INCLUDE_DIR
  NAMES AL/alut.h
)

find_library(ALUT_LIBRARY
  NAMES alut freealut
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ALUT
  FOUND_VAR ALUT_FOUND
  REQUIRED_VARS ALUT_LIBRARY ALUT_INCLUDE_DIR
)

if(ALUT_FOUND AND NOT TARGET ALUT::ALUT)
  add_library(ALUT::ALUT UNKNOWN IMPORTED)
  set_target_properties(ALUT::ALUT PROPERTIES
    IMPORTED_LOCATION "${ALUT_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${ALUT_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(ALUT_INCLUDE_DIR ALUT_LIBRARY)
