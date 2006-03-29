 
#######################################################
# Macro: FIND_HEADER_AND_SET_CONFIG_H_VALUE
# Description: find a header and set CONFIG_H_VAR to 
#              the config.h file. Macro provide
#              user friendly output.
# Parameters: INCLUDE_FILE = the header file name.
#             CONFIG_H_DEFINE = the config.h var to set
#                               if it is found.
#######################################################


MACRO( FIND_HEADER_AND_SET_CONFIG_H_VALUE  
	INCLUDE_FILE  CONFIG_H_DEFINE )
	
	FIND_FILE( H_FOUND ${INCLUDE_FILE} ${GENERIC_HEADER_DIRS} )
	IF (H_FOUND)
		MESSAGE( "-- Checking for ${INCLUDE_FILE} ........ yes" )
		WRITE_TO_CONFIG_H( ${CONFIG_H_DEFINE} 1 )
	ELSE (H_FOUND)
		MESSAGE( "-- Checking for ${INCLUDE_FILE} ........ no" )
	ENDIF (H_FOUND)
	
ENDMACRO( FIND_HEADER_AND_SET_CONFIG_H_VALUE )



#######################################################
# Macro: WRITE_TO_CONFIG_H
# Description: write definition of VAR_DEFINE
#              with VAR_VALUE to config.h file 
#              (append method).
# Parameters: VAR_DEFINE = define name.
#             VAR_VALUE = define value.
#######################################################


MACRO( WRITE_TO_CONFIG_H  
	VAR_DEFINE  VAR_VALUE )
	
	FILE( APPEND config.h "\#define ${VAR_DEFINE}	${VAR_VALUE}")
	FILE( APPEND config.h "\n")
	
ENDMACRO( WRITE_TO_CONFIG_H )