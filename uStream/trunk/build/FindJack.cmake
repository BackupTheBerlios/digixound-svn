
FIND_PATH(JACK_INCLUDE_DIR jack/jack.h
${GENERIC_HEADER_DIRS}
${JACK_CUSTOM_PATH}
${JACK_CUSTOM_PATH}/include
)

FIND_LIBRARY(JACK_LIBRARY jack
${GENERIC_LIB_DIRS}
${JACK_CUSTOM_PATH}
${JACK_CUSTOM_PATH}/lib
)

IF (JACK_LIBRARY)
  IF (JACK_INCLUDE_DIR)
    SET(JACK_LIBRARIES ${JACK_LIBRARY})
    SET(JACK_FOUND "YES")
    MESSAGE( "-- Looking for jack ............ ${JACK_LIBRARIES}")
  ENDIF (JACK_INCLUDE_DIR)
ELSE (JACK_LIBRARY)
    MESSAGE( "-- Looking for jack ............ not found!")
ENDIF (JACK_LIBRARY)

# Deprecated declarations.
SET (NATIVE_JACK_INCLUDE_PATH ${JACK_INCLUDE_DIR} )
GET_FILENAME_COMPONENT (NATIVE_JACK_LIB_PATH ${JACK_LIBRARY} PATH)
