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

#ifndef TYPES_HPP
#define TYPES_HPP

#include "typedefs.hpp"
#include "constants.hpp"

namespace uStreamLib {
	/**
	 * This enum defines data flavours. A data pin
	 * can manage only one of these data types. Two
	 * data pins must agree on data flavour to be
	 * connected.
	 */
	enum DataType { 
		DT_UNDEF = 0,    // undefined data flavour
		DT_AUDIO = 1,    // audio data
		DT_VIDEO = 2,    // video data
		DT_TEXT = 3,	// text data
		DT_BYTES = 4	 // generic byte stream
	};

	/**
	 * These are general purpose constants which define
	 * audio formats. A Source or Sink which drives an audio
	 * device must make the right configurations or conversions
	 * based on these constants.
	 */
	enum AudioFormat { 
		AF_UNDEF = 0x00000000,     // undefined (need to query)
		AF_U8 = 0x00000001, 	// unsigned 8 bit
		AF_S8 = 0x00000002, 	// signed 8 bit
		AF_S16_LE = 0x00000004, 	// signed 16 bit little endian
		AF_S16_BE = 0x00000008, 	// signed 16 bit big endian
		AF_S16_NE = 0x00000010, 	// ?
		AF_S32_LE = 0x00000020, 	// maybe stereo little endian
		AF_S32_BE = 0x00000040, 	// maybe stereo big endian
		AF_U16_LE = 0x00000080, 	// unsigned 16 bit little endian
		AF_U16_BE = 0x00000100, 	// unsigned 16 bit big endian
		AF_MLAW = 0x00000200,     // mu law
		AF_ALAW = 0x00000400,     // a law
		AF_ADPCM = 0x00000800,     // adpcm
		AF_S16_CH1 = 0x00001000,	 // 16bit signed 1 sample =  2 bytes
		AF_S16_CH2 = 0x00002000,	 // 16bit signed 1 sample =  4 bytes
		AF_S16_CH4 = 0x00004000,	 // 16bit signed 1 sample =  8 bytes
		AF_S16_CH5 = 0x00008000,	 // 16bit signed 1 sample = 10 bytes
		AF_S16_CH6 = 0x00010000 	 // 16bit signed 1 sample = 12 bytes
	};

	/**
	 * These are general purpose constants which define
	 * video formats. A Source or Sink which drives a video
	 * device must make the right configurations or conversions
	 * based on these constants (NOT COMPLETE).
	 */
	enum VideoFormat { 
		VF_UNDEF = 0x00000000,   // undefined (use fourcc field)
		VF_RGB24 = 0x00000001,   // rgb 24 bit
		VF_RGBA = 0x00000002,   // rgb 32 bit with alpha
		VF_RGB32 = 0x00000004,   // rgb 24 bit padded to 32
		VF_RGB16_565 = 0x00000008,   // rgb 16 bit 565
		VF_RGB16_555 = 0x00000010,   // rgb 16 bit 555 top bit undefined
		VF_UYVY = 0x00000020,   // yuv 4:2:2
		VF_IYU1 = 0x00000040,   // yuv 4:1:1 12bit ieee1394
		VF_IYU2 = 0x00000080,   // yuv 4:1:1 24bit ieee1394
		VF_YV12 = 0x00000100,   // yuv planar 4:2:0
		VF_NV12 = 0x00000200,   // yuv planar 4:2:0
		VF_YUV420 = 0x00000400,   // yuv 4:2:0 packed (?)
		VF_YUV422 = 0x00000800,   // yuv 4:2:2 packed (?)
		VF_YUYV = 0x00001000,   // yuv 4:2:2 packed
		VF_YUV411 = 0x00002000,   // yuv 4:1:1
		VF_RAW = 0x00004000,   // raw capture on bt848
		VF_YUV422P = 0x00008000,   // yuv 4:2:2 planar
		VF_YUV411P = 0x00010000,   // yuv 4:1:1 planar
		VF_GREY = 0x00020000,   // grey scale
		VF_IYUV = 0x00040000,   // yuv 4:2:0 planar
		VF_YUY2 = 0x00080000,   // yuv 4:2:2 packed
		VF_YVYU = 0x00100000,   // yuv 4:2:2 packed
		VF_I420 = 0x00200000,   // yuv 4:2:0 planar
		VF_YVU9 = 0x00400000,   // ?
		VF_BGR24 = 0x00800000,   // rgb 24 bit in bgr format
		VF_BGRA = 0x01000000,   // rgb 32 with alpha and bgr format
		VF_BGR32 = 0x02000000,   // rgb 32 in bgr format and 0 padding
		VF_ARGB = 0x04000000,   // rgb 32 with alpha
		VF_YUV410 = 0x08000000,   // yuv 4:1:0 (no more info)
		VF_ABGR = 0x10000000	// rgb 32 with alpha and bgr format
	};

	/**
	 * These are general purpose constants which define text formats.
	 * Must be completed.
	 */
	enum TextFormat { 
		TF_UNDEF = 0x00000000,   // undefined (need to query)
		TF_ASCII_ISL = 0x00000001,   // interchange scripting language (ISL/LBA)
		TF_ASCII_KV = 0x00000002,   // key=value formatted records
		TF_ASCII_CMSL = 0x00000004,   // config machine scripting language (CMSL)
		TF_XML = 0x00000008    // generic xml format
	};

	/**
	 * These are general purpose constants which define generic bytestream
	 * format. To be extended.
	 */
	enum ByteStreamFormat { 
		BF_UNDEF = 0x00000000,   // undefined (need to query)
		BF_AVI = 0x00000001,   // standard audio video interleaved
		BF_DNF = 0x00000002,   // uStream specific video only format
		BF_DAF = 0x00000004,   // uStream specific audio only format
		BF_DTF = 0x00000008,   // uStream specific text only format
		BF_DAVT = 0x00000010,   // uStream specific audio/video/text format
		BF_WAV = 0x00000020,   // standard wav bytestream
		BF_ISL_OVER_BIC = 0x00000040,   // uosutil isl over binary container
		BF_FLOAT_STREAM = 0x00000080,   // float (4 bytes) stream
		BF_DOUBLE_STREAM = 0x00000100,   // double (8 bytes) stream
		BF_INT32_STREAM = 0x00000200,   // signed integer (4 bytes) stream
		BF_UINT32_STREAM = 0x00000400,   // unsigned integer (4 bytes) stream
		BF_INT16_STREAM = 0x00000800,   // signed short (2 bytes) stream
		BF_UINT16_STREAM = 0x00001000,   // unsigned short (2 bytes) stream
		BF_BYTES = 0x10000000    // raw byte stream with no specific meaning
	};

	/**
	 * This enum contains RTP packet types.
	 * A type means that the packet is carring
	 * data with a specified encoding.
	 */
	enum RTPPayloadType { 
	/*
	 * uStream specific codes. These are not used in the
	 * RTP header, but only in internal structures.
	 */
	RTP_PT_DYNAMIC = -2, RTP_PT_UNDEF = -1,
	/*
	 * Rfc1889. Standard packet types. Extended later.
	 * A = Audio, V = Video, other letters have no specific meaning.
	 */
	RTP_PT_PCMU = 0, RTP_PT_1016 = 1, RTP_PT_G726_32 = 2, RTP_PT_GSM = 3,
	RTP_PT_G723 = 4, RTP_PT_DVI4_8KHz = 5, RTP_PT_DVI4_16KHz = 6,
	RTP_PT_LPC = 7, RTP_PT_PCMA = 8, RTP_PT_G722 = 9, RTP_PT_L16_stereo = 10,
	RTP_PT_L16_mono = 11, RTP_PT_QCELP = 12, RTP_PT_UNUSED_A01 = 13,
	RTP_PT_MPA = 14, RTP_PT_G728 = 15, RTP_PT_DVI4_11KHz = 16,
	RTP_PT_DVI4_22KHz = 17, RTP_PT_G729 = 18, RTP_PT_CN = 19,
	RTP_PT_UNUSED_A02 = 20, RTP_PT_UNUSED_A03 = 21, RTP_PT_UNUSED_A04 = 22,
	RTP_PT_UNUSED_A05 = 23, RTP_PT_UNUSED_V01 = 24, RTP_PT_CELB = 25,
	RTP_PT_JPEG = 26, RTP_PT_UNUSED_V02 = 27, RTP_PT_NV = 28,
	RTP_PT_UNUSED_V03 = 29, RTP_PT_UNUSED_V04 = 30, RTP_PT_H261 = 31,
	RTP_PT_MPV = 32, RTP_PT_MP2T = 33, RTP_PT_H263 = 34,
	RTP_PT_UNUSED_X01 = 35, RTP_PT_UNUSED_X37 = 71, RTP_PT_RESERV_X01 = 72, // reserved range
	RTP_PT_RESERV_X05 = 76, // from 72 to 76

	RTP_PT_UNUSED_Y01 = 77, RTP_PT_UNUSED_Y19 = 95, RTP_PT_DYNAMIC_01 = 96, // dynamic range
	RTP_PT_DYNAMIC_32 = 127  // from 96 to 127
	};

	/**
	 * Object types for uStream library.
	 */
	enum RTTIuStream { UOSUTIL_RTTI_WIRE = UOSUTIL_RTTI_LAST_ID, UOSUTIL_RTTI_PIN,
	UOSUTIL_RTTI_DATA_PIN, UOSUTIL_RTTI_CONTROL_PIN, UOSUTIL_RTTI_BLOCK,
	UOSUTIL_RTTI_SOURCE, UOSUTIL_RTTI_SINK, UOSUTIL_RTTI_FILTER,
	UOSUTIL_RTTI_BLOCK_MANAGER, UOSUTIL_RTTI_AUDIO_FORMATS,
	UOSUTIL_RTTI_VIDEO_FORMATS, UOSUTIL_RTTI_TEXT_FORMATS,
	UOSUTIL_RTTI_BYTE_FORMATS, UOSUTIL_RTTI_STATUS_LISTENER, UOSUTIL_RTTI_EVENT,
	UOSUTIL_RTTI_EVENT_SOURCE, UOSUTIL_RTTI_EVENT_LISTENER,
	UOSUTIL_RTTI_EVENT_BLOCK, UOSUTIL_RTTI_EVENT_BLOCK_LISTENER,
	UOSUTIL_RTTI_EVENT_BLOCK_SOURCE };
}

#endif
