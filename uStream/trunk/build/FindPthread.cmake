
FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
${GENERIC_HEADER_DIRS}
${PTHREAD_CUSTOM_PATH}
${PTHREAD_CUSTOM_PATH}/include
)

FIND_LIBRARY(PTHREAD_LIBRARY pthread
${GENERIC_LIB_DIRS}
${PTHREAD_CUSTOM_PATH}
${PTHREAD_CUSTOM_PATH}/lib
)

IF (PTHREAD_LIBRARY)
  IF (PTHREAD_INCLUDE_DIR)
    SET(PTHREAD_LIBRARIES ${PTHREAD_LIBRARY})
    SET(PTHREAD_FOUND "YES")
    MESSAGE( "-- Looking for libpthread ............ ${PTHREAD_LIBRARIES}")
  ENDIF (PTHREAD_INCLUDE_DIR)
ELSE (PTHREAD_LIBRARY)
    MESSAGE( "-- Looking for libpthread ............ not found!")
ENDIF (PTHREAD_LIBRARY)

# Deprecated declarations.
SET (NATIVE_PTHREAD_INCLUDE_PATH ${PTHREAD_INCLUDE_DIR} )
GET_FILENAME_COMPONENT (NATIVE_PTHREAD_LIB_PATH ${PTHREAD_LIBRARY} PATH)
