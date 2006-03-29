/*
  uSTREAM LIGHT-WEIGHT STREAMING ARCHITECTURE
  Copyright (C) 2005 Luis Serrano (luis@kontrol-dj.com) 
  
  Based on DANUBIO STREAMING ARCHITECTURE by Michele Iacobellis (m.iacobellis@nexotech.it)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "typedefs.hpp"
#include "logger.hpp"

/*
 * Export macro used under Windows.
 */
#if defined(_WIN32) || defined(WIN32)
#if defined(LIBUS_USTREAM_EXPORTS)
#define US_EXPORT __declspec(dllexport)
#else
#define US_EXPORT __declspec(dllimport)
#endif
#else
#define US_EXPORT
#endif

/*
 * Version information.
 */

#define USTREAM_VERSION_MAJOR		  1
#define USTREAM_VERSION_MINOR		  0
#define USTREAM_VERSION_MICRO		  0
#define USTREAM_VERSION_PATCH		  0

/*
 * Default parameters.
 */

/* Action scheduler timeout (in milliseconds) */
#define US_DEFAULT_BM_ASTIMEOUT		 10

/* Plugin check timeout (in seconds) */
#define US_DEFAULT_BM_PCTIMEOUT		 60

/* Logger level (from DEBUG to EMERG) */
#define US_DEFAULT_BM_LOGLEVEL		 Logger::LEVEL_WARN

/* Logger mode (one of FORCE or DEFER) */
#define US_DEFAULT_BM_LOGMODE		 Logger::MODE_FORCE

/*
 * Numeric constants.
 */

/* Priority levels in a Block queue for inter thread communication */
#define US_PRIORITY_LEVELS			  1

/* Out of band queue's size */
#define US_OOB_QUEUE_SIZE			 30

/* Preferred ControlPin's buffer size */
#define US_CP_BUFSZ				4096

/* Preferred ControlPin's buffers count */
#define US_CP_BUFCO				 10

/* ControlPin's internal priority queue size */
#define US_CP_QUEUESZ				 50

/* Block's status messages queue size */
#define US_STATUS_QSZ				 50

/* Count of hash buckets in I/O datapins tables */
#define US_DPT_HSIZE				 37

/* Count of hash buckets in ConfigTables (very high) */
#define US_CONFIGTABLE_HSIZE			 67

/* Count of hash buckets in Blocks table */
#define US_BLOCKTABLE_HSIZE 			 67

/* Maximum number of logged lines (each line of 256 bytes)  */
#define US_LOGGER_LINESIZE  			256

/* Blocks' error string size */
#define US_BLOCK_ERRORSTRINGSZ			256

/*
 * Strings.
 */

/* Default system log file path */
#define US_LOGGER_FILEPATH  		 "uStream.log"

/*
 * Configuration properties.
 */

#define USBM_ACTIONSCHEDULER_TIMEOUT	   "uStream.SchedulerTimeout"
#define USBM_LOGGER_LEVEL		"uStream.LoggerLevel"

/*
 * Predefined for block (common to all blocks).
 */

#define USPL_I_PREFBUFSZ	"PreferredBufferSizeI"
#define USPL_I_PREFBUFCO	"PreferredBuffersCountI"
#define USPL_I_QUEUESZ		"QueueSizeI"

#define USPL_O_PREFBUFSZ	"PreferredBufferSizeO"
#define USPL_O_PREFBUFCO	"PreferredBuffersCountO"
#define USPL_O_QUEUESZ		"QueueSizeO"

/*
 * Predefined for block (common to all blocks).
 */

#define USBL_DGSSKIP		 "_DGSSkip"

#endif
