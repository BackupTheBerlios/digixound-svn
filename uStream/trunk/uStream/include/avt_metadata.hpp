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

#ifndef AVTMETADATA_HPP
#define AVTMETADATA_HPP

#include "types.hpp"
#include "timer.hpp"

namespace uStreamLib {
	/**
	 * Audio/Video/Text metadata.
	 * This structure contains meta information about
	 * data sent by data pins. A source or filter must
	 * fill these data correctly so that a sink can perform
	 * its tasks.
	 */
	union US_EXPORT avt_metadata_t {
		/** info about audio data */
		struct audio_info_t {
			/** rtp type code */
			uint8 rtptype;

			/** bit per sample */
			uint8 bitspersample;

			/** number of channels */
			uint8 n_channels;

			/** sample frequency */
			uint32 rate;

			/** audio bytes format */
			AudioFormat af;
		} audio_info;

		/** info about video data */
		struct video_info_t {
			/** fourcc code word */
			uint32 fourcc;

			/** rtp type code */
			uint8 rtptype;

			/** bit depth */
			uint8 depth;

			/** frames per second */
			uint16 framerate;

			/** horizontal resolution */
			uint16 width;

			/** vertical resolution */
			uint16 height;

			/** video bytes format (chroma) */
			VideoFormat vf;
		} video_info;

		/** info about text data */
		struct text_info_t {
			/** user tag */
			uint32 tag;

			/** text format */
			TextFormat tf;
		} text_info;

		/** generic byte stream info */
		struct bytestream_info_t {
			/** flag: bytestream contains audio */
			uint32 hasaudio : 1;

			/** flag: bytestream contains video */
			uint32 hasvideo : 1;

			/** flag: bytestream contains text */
			uint32 hastext : 1;

			/** unsed: reserved for extensions */
			uint32 reserved : 29;

			/** user tag */
			uint32 tag;

			/** user item counter */
			uint32 count;

			/** bytestream format */
			ByteStreamFormat bf;
		} bytestream_info;
	};

	typedef union avt_metadata_t avt_metadata;

	/**
	 * Data information structure.
	 * When a source emits one frame per buffer, it must
	 * set the isframe flag to true and n_chunks to 1. If a
	 * source does not emit one frame per buffer, it must set
	 * isframe flag to false and then set n_chunks to the number of
	 * chunks needed for building a single frame. This is valid
	 * for video and audio sources and filters (remeber that a filter
	 * is a source too, of course).
	 * A source or filter probes framesize using the vf or af format
	 * identifier. It must set framesize according to that parameter.
	 * For example, if vf is VF_YUV420, then framesize is w*h*3/2, where
	 * w and h are horizontal and vertical resolution of the video.
	 * If a single produced buffer is 4096 and framesize is 115200 (w = 320,
	 * h = 240), then 115200/4096=28.125. n_chunks must be set to 29 and the
	 * last buffer will have 512 valid bytes. Use getCount() on the produced
	 * DataBuf to get count of valid bytes in the buffer.
	 * This structure is valid for audio too.
	 */
	struct US_EXPORT datainfo_t {
		/** data type */
		DataType dt;

		/** flag: buffer is a single frame */
		bool isframe;

		/** number of chunks to make a frame */
		uint32 n_chunks;

		/** frame size as probed by source/filter */
		uint32 framesize;

		/** time description */
		TimeDesc td;

		/** flag: this buffer is a key frame */
		bool iskeyframe;
	};

	typedef struct datainfo_t datainfo;

	/**
	 * Skip and Seek information structure.
	 * This structure is used to specify seek and skip info.
	 * Seeking and Skipping can be performed on a bytes basis
	 * or specifing time information that a specific block handler
	 * must parse and use.
	 */
	struct US_EXPORT seekinfo_t {
		/** seek position in bytes */
		int32 seekpos;

		/** seek position in hours */
		int32 hour;

		/** seek position in minutes */
		int32 minutes;

		/** seek position in seconds */
		int32 seconds;

		/** seek position in frames */
		int32 frames;

		/** seek position in subframes */
		int32 subframes;
	};

	typedef struct seekinfo_t seekinfo;
}

#endif
