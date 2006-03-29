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

#include "formats_video.hpp"

namespace uStreamLib {
	VideoFormats::VideoFormats(void)
	{
		setClassID(UOSUTIL_RTTI_VIDEO_FORMATS);
	}

	VideoFormats::~VideoFormats(void)
	{
		// nothing to do
	}

	int32 VideoFormats::init(uint32 hsize)
	{
		int32 tsz = 0, ret = 0;

		struct vf_itm {
			item itm;
			char* vfstring;
		};

		struct vf_itm itms[] = {
			{{ VIDEOFORMAT, 0, 0,      VF_UNDEF, 32 },     "VF_UNDEF" },
			{{ VIDEOFORMAT, 0, 0,  	VF_RGB24, 24 }, 	"VF_RGB24" },
			{{ VIDEOFORMAT, 0, 0,   	VF_RGBA, 32 },  	"VF_RGBA" },
			{{ VIDEOFORMAT, 0, 0,  	VF_RGB32, 32 }, 	"VF_RGB32" },
			{{ VIDEOFORMAT, 0, 0,  VF_RGB16_565, 16 }, "VF_RGB16_565" },
			{{ VIDEOFORMAT, 0, 0,  VF_RGB16_555, 16 }, "VF_RGB16_555" },
			{{ VIDEOFORMAT, 0, 0,   	VF_UYVY, 16 },  	"VF_UYUV" },
			{{ VIDEOFORMAT, 0, 0,   	VF_IYU1, 12 },  	"VF_IYU1" },
			{{ VIDEOFORMAT, 0, 0,   	VF_IYU2, 24 },  	"VF_IYU2" },
			{{ VIDEOFORMAT, 0, 0,   	VF_YV12, 12 },  	"VF_YV12" },
			{{ VIDEOFORMAT, 0, 0,   	VF_NV12, 12 },  	"VF_NV12" },
			{{ VIDEOFORMAT, 0, 0,     VF_YUV420, 12 },    "VF_YUV420" },
			{{ VIDEOFORMAT, 0, 0, 	VF_YUV422, 16 },	"VF_YUV422" },
			{{ VIDEOFORMAT, 0, 0,   	VF_YUYV, 16 },  	"VF_YUYV" },
			{{ VIDEOFORMAT, 0, 0,     VF_YUV411, 16 },    "VF_YUV411" },
			{{ VIDEOFORMAT, 0, 0,   	 VF_RAW, 32 },  	 "VF_RAW" },
			{{ VIDEOFORMAT, 0, 0,    VF_YUV422P, 16 },   "VF_YUV422P" },
			{{ VIDEOFORMAT, 0, 0,    VF_YUV411P, 16 },   "VF_YUV411P" },
			{{ VIDEOFORMAT, 0, 0,   	VF_GREY,  8 },  	"VF_GREY" },
			{{ VIDEOFORMAT, 0, 0,   	VF_IYUV, 12 },  	"VF_IYUV" },
			{{ VIDEOFORMAT, 0, 0,   	VF_YUY2, 16 },  	"VF_YUY2" },
			{{ VIDEOFORMAT, 0, 0,   	VF_YVYU, 16 },  	"VF_YVYU" },
			{{ VIDEOFORMAT, 0, 0,   	VF_I420, 12 },  	"VF_I420" },
			{{ VIDEOFORMAT, 0, 0,   	VF_YVU9, 36 },  	"VF_YVU9" },
			{{ VIDEOFORMAT, 0, 0,  	VF_BGR24, 24 }, 	"VF_BGR24" },
			{{ VIDEOFORMAT, 0, 0,   	VF_BGRA, 32 },  	"VF_BGRA" },
			{{ VIDEOFORMAT, 0, 0,  	VF_BGR32, 32 }, 	"VF_BGR32" },
			{{ VIDEOFORMAT, 0, 0,   	VF_ARGB, 32 },  	"VF_ARGB" },
			{{ VIDEOFORMAT, 0, 0,     VF_YUV410, 16 },    "VF_YUV410" },
			{{ VIDEOFORMAT, 0, 0,   	VF_ABGR, 32 },  	"VF_ABGR" }
		};

		ret = SharedHash::init(hsize);
		if (ret == FAILURE)
			return FAILURE;

		tsz = sizeof(itms) / sizeof(vf_itm);
		for (int32 i = 0; i < tsz; i++) {
			put(itms[i].vfstring, (char *) &itms[i].itm, sizeof(itms[i].itm));
		}

		return SUCCESS;
	}

	void VideoFormats::build(int32 what, int16 min_w, int16 min_h,
		int16 max_w, int16 max_h)
	{
		int32 i = 0;
		int32 max_rv_idx = 26, max_rd_idx = 8;
		char tmp[4096];

		item r_v[] = {
			{ RESOLUTION,    1,    1, VF_UNDEF, 0 }, //  0
			{ RESOLUTION,   32,   24, VF_UNDEF, 0 }, //  1
			{ RESOLUTION,   64,   48, VF_UNDEF, 0 }, //  2
			{ RESOLUTION,   80,   60, VF_UNDEF, 0 }, //  3
			{ RESOLUTION,  128,   96, VF_UNDEF, 0 }, //  4
			{ RESOLUTION,  160,  100, VF_UNDEF, 0 }, //  5
			{ RESOLUTION,  160,  120, VF_UNDEF, 0 }, //  6
			{ RESOLUTION,  176,  144, VF_UNDEF, 0 }, //  7
			{ RESOLUTION,  192,  144, VF_UNDEF, 0 }, //  8
			{ RESOLUTION,  300,  185, VF_UNDEF, 0 }, //  9
			{ RESOLUTION,  320,  200, VF_UNDEF, 0 }, // 10
			{ RESOLUTION,  320,  240, VF_UNDEF, 0 }, // 11
			{ RESOLUTION,  352,  288, VF_UNDEF, 0 }, // 12
			{ RESOLUTION,  384,  288, VF_UNDEF, 0 }, // 13
			{ RESOLUTION,  352,  480, VF_UNDEF, 0 }, // 14
			{ RESOLUTION,  352,  576, VF_UNDEF, 0 }, // 15
			{ RESOLUTION,  480,  576, VF_UNDEF, 0 }, // 16
			{ RESOLUTION,  640,  262, VF_UNDEF, 0 }, // 17
			{ RESOLUTION,  640,  480, VF_UNDEF, 0 }, // 18
			{ RESOLUTION,  704,  576, VF_UNDEF, 0 }, // 19
			{ RESOLUTION,  720,  480, VF_UNDEF, 0 }, // 20
			{ RESOLUTION,  720,  576, VF_UNDEF, 0 }, // 21
			{ RESOLUTION,  800,  600, VF_UNDEF, 0 }, // 22
			{ RESOLUTION, 1152,  864, VF_UNDEF, 0 }, // 23
			{ RESOLUTION, 1408, 1152, VF_UNDEF, 0 }, // 24
			{ RESOLUTION, 1920, 1080, VF_UNDEF, 0 }  // 25

		};

		item r_d[] = {
			{ RESOLUTION,  320, 200, VF_UNDEF, 0 },
			{ RESOLUTION,  640, 480, VF_UNDEF, 0 },
			{ RESOLUTION,  800, 600, VF_UNDEF, 0 },
			{ RESOLUTION, 1024, 768, VF_UNDEF, 0 },
			{ RESOLUTION, 1152, 864, VF_UNDEF, 0 },
			{ RESOLUTION, 1280,1024, VF_UNDEF, 0 },
			{ RESOLUTION, 1400,1050, VF_UNDEF, 0 },
			{ RESOLUTION, 1600,1280, VF_UNDEF, 0 }
		};

		switch (what) {
		case RES_VIDEO:
			for (i = 0; i < max_rv_idx; i++) {
				if ((r_v[i].w >= min_w || r_v[i].h >= min_h) &&
					(r_v[i].w <= max_w || r_v[i].h <= max_h)) {
					snprintf(tmp, sizeof(tmp), "%dx%d", r_v[i].w, r_v[i].h);
					put(tmp, (char *) &r_v[i], sizeof(item));
				}
			}
			break;
		case RES_DISPLAY:
			for (i = 0; i < max_rd_idx; i++) {
				if ((r_d[i].w >= min_w || r_d[i].h >= min_h) &&
					(r_d[i].w <= max_w || r_d[i].h <= max_h)) {
					snprintf(tmp, sizeof(tmp), "%dx%d", r_d[i].w, r_d[i].h);
					put(tmp, (char *) &r_d[i], sizeof(item));
				}
			}
		}
	}

	void VideoFormats::fillVectorWithResolutions(Vector* v)
	{
		Enumeration* en = keys();
		while (en->hasMoreElements()) {
			char* key = (char*) en->nextElement();
			item* itm = (item*) get(key);

			if (itm->type == RESOLUTION)
				v->addElement(key, (uint32) strlen(key) + 1);
		}
	}

	void VideoFormats::fillVectorWithFormats(Vector* v)
	{
		Enumeration* en = keys();
		while (en->hasMoreElements()) {
			char* key = (char*) en->nextElement();
			item* itm = (item*) get(key);

			if (itm->type == VIDEOFORMAT)
				v->addElement(key, (uint32) strlen(key) + 1);
		}
	}

	uint32 VideoFormats::computeImageSize(VideoFormat vf, int16 w, int16 h)
	{
		switch (vf) {
		case VF_UNDEF:
			return w * h * 4;     // use RGB32 by default
		case VF_RGB24:
			return w * h * 3;
		case VF_RGBA:
			return w * h * 4;
		case VF_RGB32:
			return w * h * 4;
		case VF_RGB16_565:
			return w * h * 2;
		case VF_RGB16_555:
			return w * h * 2;
		case VF_UYVY:
			return w * h * 2;     // as YUV422
		case VF_IYU1:
			return w * h * 3 / 2;
		case VF_IYU2:
			return w * h * 3;
		case VF_YV12:
			return w * h * 3 / 2; // as YUV420
		case VF_NV12:
			return w * h * 3 / 2; // as YUV420
		case VF_YUV420:
			return w * h * 3 / 2;
		case VF_YUV422:
			return w * h * 2;
		case VF_YUYV:
			return w * h * 2;     // as YUV422
		case VF_YUV411:
			return w * h * 2;     // uncertain
		case VF_RAW:
			return w * h * 4;     // use RGB32 by default
		case VF_YUV422P:
			return w * h * 2;     // as YUV422
		case VF_YUV411P:
			return w * h * 2;
		case VF_GREY:
			return w * h;
		case VF_IYUV:
			return w * h * 3 / 2; // as YUV420
		case VF_YUY2:
			return w * h * 2;     // as YUV422
		case VF_YVYU:
			return w * h * 2;     // as YUV422
		case VF_I420:
			return w * h * 3 / 2; // as YUV420
		case VF_YVU9:
			return w * h * 9 / 8; // uncertain
		case VF_BGR24:
			return w * h * 3;
		case VF_BGRA:
			return w * h * 4;
		case VF_BGR32:
			return w * h * 4;
		case VF_ARGB:
			return w * h * 4;
		case VF_YUV410:
			return w * h * 2; // uncertain
		case VF_ABGR:
			return w * h * 4;
		}

		return 0;
	}

	uint8 VideoFormats::getDepth(VideoFormat vf)
	{
		switch (vf) {
		case VF_UNDEF:
			return 32; // use RGB32 by default
		case VF_RGB24:
			return 24;
		case VF_RGBA:
			return 32;
		case VF_RGB32:
			return 32;
		case VF_RGB16_565:
			return 16;
		case VF_RGB16_555:
			return 16;
		case VF_UYVY:
			return 16;
		case VF_IYU1:
			return 12;
		case VF_IYU2:
			return 24;
		case VF_YV12:
			return 12;
		case VF_NV12:
			return 12;
		case VF_YUV420:
			return 12;
		case VF_YUV422:
			return 16;
		case VF_YUYV:
			return 16;
		case VF_YUV411:
			return 16; // uncertain
		case VF_RAW:
			return 32; // use RGB32 by default
		case VF_YUV422P:
			return 16;
		case VF_YUV411P:
			return 16;
		case VF_GREY:
			return  8;
		case VF_IYUV:
			return 12; // as I420
		case VF_YUY2:
			return 16;
		case VF_YVYU:
			return 16;
		case VF_I420:
			return 12;
		case VF_YVU9:
			return 36; // uncertain
		case VF_BGR24:
			return 24;
		case VF_BGRA:
			return 32;
		case VF_BGR32:
			return 32;
		case VF_ARGB:
			return 32;
		case VF_YUV410:
			return 16; // uncertain
		case VF_ABGR:
			return 32;
		}

		return 0;
	}

	uint32 VideoFormats::computeFourCC(VideoFormat vf)
	{
		/*
		  * These fourcc codes must be checked against official
		  * ones. They are not all correct.
		  */

		switch (vf) {
		case VF_UNDEF:
			return 0;
		case VF_RGB24:
			return MAKEFOURCC('R', 'G', 'B', ' ');
		case VF_RGBA:
			return MAKEFOURCC('R', 'G', 'B', 'A');
		case VF_RGB32:
			return MAKEFOURCC('R', 'G', 'B', 'A');
		case VF_RGB16_565:
			return MAKEFOURCC('R', 'G', 'B', '2');
		case VF_RGB16_555:
			return MAKEFOURCC('R', 'G', 'B', '2');
		case VF_UYVY:
			return MAKEFOURCC('U', 'Y', 'V', 'Y');
		case VF_IYU1:
			return MAKEFOURCC('I', 'Y', 'U', '1');
		case VF_IYU2:
			return MAKEFOURCC('I', 'Y', 'U', '2');
		case VF_YV12:
			return MAKEFOURCC('Y', 'V', '1', '2');
		case VF_NV12:
			return MAKEFOURCC('N', 'V', '1', '2');
		case VF_YUV420:
			return MAKEFOURCC('I', '4', '2', '0');
		case VF_YUV422:
			return MAKEFOURCC('I', '4', '2', '2');
		case VF_YUYV:
			return MAKEFOURCC('Y', 'U', 'Y', 'V');
		case VF_YUV411:
			return MAKEFOURCC('I', '4', '1', '1');
		case VF_RAW:
			return MAKEFOURCC('R', 'A', 'W', ' ');
		case VF_YUV422P:
			return MAKEFOURCC('I', '4', '2', '2');
		case VF_YUV411P:
			return MAKEFOURCC('I', '4', '1', '1');
		case VF_GREY:
			return MAKEFOURCC('G', 'R', 'E', 'Y');
		case VF_IYUV:
			return MAKEFOURCC('I', 'Y', 'U', 'V');
		case VF_YUY2:
			return MAKEFOURCC('Y', 'U', 'Y', '2');
		case VF_YVYU:
			return MAKEFOURCC('Y', 'V', 'Y', 'U');
		case VF_I420:
			return MAKEFOURCC('I', '4', '2', '0');
		case VF_YVU9:
			return MAKEFOURCC('Y', 'V', 'U', '9');
		case VF_BGR24:
			return MAKEFOURCC('B', 'G', 'R', ' ');
		case VF_BGRA:
			return MAKEFOURCC('B', 'G', 'R', 'A');
		case VF_BGR32:
			return MAKEFOURCC('B', 'G', 'R', 'A');
		case VF_ARGB:
			return MAKEFOURCC('A', 'R', 'G', 'B');
		case VF_YUV410:
			return MAKEFOURCC('I', '4', '1', '0');
		case VF_ABGR:
			return MAKEFOURCC('A', 'B', 'G', 'R');
		}

		return 0;
	}

	char* VideoFormats::getVideoFormatString(VideoFormat vf)
	{
		switch (vf) {
		case VF_UNDEF:
			return "VF_UNDEF";
		case VF_RGB24:
			return "VF_RGB24";
		case VF_RGBA:
			return "VF_RGBA";
		case VF_RGB32:
			return "VF_RGB32";
		case VF_RGB16_565:
			return "VF_RGB16_565";
		case VF_RGB16_555:
			return "VF_RGB16_555";
		case VF_UYVY:
			return "VF_UYUV";
		case VF_IYU1:
			return "VF_IYU1";
		case VF_IYU2:
			return "VF_IYU2";
		case VF_YV12:
			return "VF_YV12";
		case VF_NV12:
			return "VF_NV12";
		case VF_YUV420:
			return "VF_YUV420";
		case VF_YUV422:
			return "VF_YUV422";
		case VF_YUYV:
			return "VF_YUYV";
		case VF_YUV411:
			return "VF_YUV411";
		case VF_RAW:
			return "VF_RAW";
		case VF_YUV422P:
			return "VF_YUV422P";
		case VF_YUV411P:
			return "VF_YUV411P";
		case VF_GREY:
			return "VF_GREY";
		case VF_IYUV:
			return "VF_IYUV";
		case VF_YUY2:
			return "VF_YUY2";
		case VF_YVYU:
			return "VF_YVYU";
		case VF_I420:
			return "VF_I420";
		case VF_YVU9:
			return "VF_YVU9";
		case VF_BGR24:
			return "VF_BGR24";
		case VF_BGRA:
			return "VF_BGRA";
		case VF_BGR32:
			return "VF_BGR32";
		case VF_ARGB:
			return "VF_ARGB";
		case VF_YUV410:
			return "VF_YUV410";
		case VF_ABGR:
			return "VF_ABGR";
		}

		return "UNDEFINED";
	}
}
