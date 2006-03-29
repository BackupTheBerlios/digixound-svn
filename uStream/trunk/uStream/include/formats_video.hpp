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

#ifndef VIDEOFORMATS_HPP
#define VIDEOFORMATS_HPP

#include "shared_hash.hpp"
#include "vector.hpp"
#include "types.hpp"

namespace uStreamLib {
	/**
	 * This class allows to manage media formats and resolutions, giving
	 * a string rapresentation such as "800x600" for the latter. It can
	 * fill a resolution vector with strings of that type,
	 * the min_w,min_h and max_w,max_h given. It can also fill a vector
	 * with video format strings.
	 */
	class US_EXPORT VideoFormats : public SharedHash {
	public:
		/**
		 * Values for what in the "item" descriptor.
		 */
		enum { VIDEOFORMAT = 1, RESOLUTION = 2 };

		/**
		 * Resolution descriptor. It contains info about
		 * a specific resolution.
		 */
		typedef struct {
			/** what is this (a video format or a resolution) */
			int8 type;

			/** width */
			int16 w;

			/** height */
			int16 h;

			/** video format identifier */
			VideoFormat vf;

			/** video format depth */
			uint8 depth;
		} item;

		/**
		 * Constants that define what type of resolution
		 * descriptors to produce.
		 */
		enum { RES_VIDEO = 1, RES_DISPLAY = 2 };

		/**
		 * Constructor.
		 */
		VideoFormats(void);

		/**
		 * Destructor.
		 */
		virtual ~VideoFormats(void);

		/**
		 * Create a video formats table.
		 * @param hsize has buckets count to store resolution
		 * descriptors and video formats descriptors.
		 */
		int32 init(uint32 hsize = 39);

		/**
		 * Build resolutions giving min_w,min_h and max_w,max_h.
		 * Resolutions are computed following typical criteria.
		 * Followed standards are those of the CIF format and
		 * typical VHS/VCD/DVD media streams for RES_VIDEO and
		 * typical display formats for RES_DISPLAY.
		 * @param type specify one of RES_VIDEO or RES_DISPLAY to
		 * build resolutions for those video or display/screen.
		 * @param min_w minimum width of resolutions to produce.
		 * @param min_g minimum height of resolutions to produce.
		 * @param max_w maximum width.
		 * @param max_h maximum height;
		 */
		void build(int32 what, int16 min_w, int16 min_h, int16 max_w,
			int16 max_h);

		/**
		 * Method to fill a vector with resolution strings.
		 */
		void fillVectorWithResolutions(Vector* v);

		/**
		 * Method to fill a vector with video format strings.
		 */
		void fillVectorWithFormats(Vector* v);

		/**
		 * Get resolution descriptor.
		 * @param res resolution string.
		 * @return a resolution descriptor or NULL.
		 */
		item* getResolution(char* res)
		{
			return (item *) get(res);
		}

		/**
		 * Get video format id, given its string rapresentation.
		 * @param vfstring the string rapresentation of video format.
		 * @return the format id.
		 */
		VideoFormat getVideoFormat(char* vfstring)
		{
			item* i = (item*) get(vfstring);
			if (i)
				return i->vf;

			return VF_UNDEF;
		}

		/**
		 * Compute image size based on video format and resolution.
		 * @param vf video format (one of the constants in the enum).
		 * @param w image width.
		 * @param h image height.
		 * @return image size or 0 if vf is undefined or unknown.
		 */
		static uint32 computeImageSize(VideoFormat vf, int16 w, int16 h);

		/**
		 * Get bits per pixel given the video format identifier.
		 * @param vf a video format id.
		 * @return depth expressed in bits per pixel.
		 */
		static uint8 VideoFormats::getDepth(VideoFormat vf);

		/**
		 * Compute FOURCC code.
		 * @param vf The video format as known by uStream.
		 * @return the fourcc code.
		 */
		static uint32 computeFourCC(VideoFormat vf);

		/**
		 * Method to get a string rapresentation of video format.
		 * @param vf video format.
		 * @return a string form of the specified video format.
		 */
		static char* getVideoFormatString(VideoFormat vf);
	private:
		/* copy constructor not available */
		VideoFormats(VideoFormats&)
		{
		}
	};
}

#endif
