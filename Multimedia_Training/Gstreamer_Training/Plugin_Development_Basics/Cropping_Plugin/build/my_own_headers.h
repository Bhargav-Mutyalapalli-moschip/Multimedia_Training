#define DEFAULT_SIZE 1920
#define DEF_START 0
#define DEF_END 2147483647
#define DEF_WVAL 0
#define DEF_HVAL 0
#define DEF_XVAL 0
#define DEF_YVAL 0
#define DEF_XSTART 0
#define DEF_XEND 2147483647
#define DEF_YSTART 0
#define DEF_YEND 2147483647

/*----------------Gstreamer Variable declarartions----------------------*/
	/*------------------------------------------------------------*/
	GstBuffer *my_buffer = NULL;
	GstVideoInfo org_video_info;
	GstVideoInfo pre_video_info;
	GstVideoFrame org_vframe,pre_vframe;
	GstCaps *org_caps=NULL;
	GstCaps *pre_caps=NULL;
	char *format_str = NULL;


	/*--------Own buffer variable declarations------------------*/
	//plane retrieval for single plane
	guint8 *org_n_pixels=NULL;
	guint8 *org_rgb_pixel=NULL; //accessing variable 
	
	//plane retrieval for two plane
	guint8 *org_y_pixels=NULL;
	guint8 *org_uv_pixels=NULL;
	guint8 *org_y_pixel=NULL;
	guint8 *org_uv_pixel=NULL;

	//plane retrieval for three plane
	guint8 *org_u_pixels=NULL;
	guint8 *org_v_pixels=NULL;
	guint8 *org_u_pixel=NULL;
	guint8 *org_v_pixel=NULL;
	
	
	//stride retrieval for single plane
	guint org_rgb_stride=0;

	//stride retrieval for two plane
	guint org_y_stride=0;
	guint org_uv_stride=0;

	//stride retrieval for three plane
	guint org_u_stride=0;
	guint org_v_stride=0;

	//pixel stride retrieval for single plane and two plane
	guint org_pixel_stride = 0;

	//pixel stride retrieval for three plane
	guint org_pixel_stride_y = 0; // pixel stride for Y plane in YV12
	guint org_pixel_stride_u = 0; // pixel stride for U plane in YV12
	guint org_pixel_stride_v = 0; // pixel stride for V plane in YV12
	
	/*----Basic caps variables---*/
	gint org_width=0;
	gint org_height=0;
        /*--------video frame size calculation variables-----*/
	gint org_size=0;
	gint org_y_size=0;
	gint org_u_size=0;
	gint org_v_size=0;
	gint org_e_size=0;

	gint org_no_planes=0;
	gint org_bpp=0;
	gint org_format_no=0;

	/*---------------------------------------------------------------------------*/

	/*--------Predefined buffer variable declarations------------------*/
	//plane retrieval for single plane
	guint8 *pre_n_pixels=NULL;
	guint8 *pre_rgb_pixel=NULL; //accessing variable 
	
	//plane retrieval for two plane
	guint8 *pre_y_pixels=NULL;
	guint8 *pre_uv_pixels=NULL;
	guint8 *pre_y_pixel=NULL;
	guint8 *pre_uv_pixel=NULL;

	//plane retrieval for three plane
	guint8 *pre_u_pixels=NULL;
	guint8 *pre_v_pixels=NULL;
	guint8 *pre_u_pixel=NULL;
	guint8 *pre_v_pixel=NULL;
	
	
	//stride retrieval for single plane
	guint pre_rgb_stride=0;

	//stride retrieval for two plane
	guint pre_y_stride=0;
	guint pre_uv_stride=0;

	//stride retrieval for three plane
	guint pre_u_stride=0;
	guint pre_v_stride=0;

	//pixel stride retrieval for single plane and two plane
	guint pre_pixel_stride = 0;

	//pixel stride retrieval for three plane
	guint pre_pixel_stride_y = 0; // pixel stride for Y plane in YV12
	guint pre_pixel_stride_u = 0; // pixel stride for U plane in YV12
	guint pre_pixel_stride_v = 0; // pixel stride for V plane in YV12
	
	/*----Basic caps variables---*/
	gint pre_width=0;
	gint pre_height=0;
        /*--------video frame size calculation variables-----*/
	gint pre_size=0;

	gint pre_no_planes=0;
	gint pre_bpp=0;
	gint pre_format_no=0;

	/*---------------------------------------------------------------------------*/

	/*---General declarations--------*/
	gint corner_x = 0;
	gint corner_y = 0;
	gint rect_bottom = 0;
	gint rect_right = 0;
	gint ind=0;
	gint denom1=0;
	gint denom2=0;




/*------------------Basic video formats macros---------------------------*/
	/*
	   GST_VIDEO_FORMAT_UNKNOWN (0) – Unknown or unset video format id
	   GST_VIDEO_FORMAT_ENCODED (1) – Encoded video format. 
	   GST_VIDEO_FORMAT_I420 (2) – planar 4:2:0 YUV
	   GST_VIDEO_FORMAT_YV12 (3) – planar 4:2:0 YVU (like I420 but UV planes swapped)
	   GST_VIDEO_FORMAT_YUY2 (4) – packed 4:2:2 YUV (Y0-U0-Y1-V0 Y2-U2-Y3-V2 Y4 ...)
	   GST_VIDEO_FORMAT_UYVY (5) – packed 4:2:2 YUV (U0-Y0-V0-Y1 U2-Y2-V2-Y3 U4 ...)
	   GST_VIDEO_FORMAT_AYUV (6) – packed 4:4:4 YUV with alpha channel (A0-Y0-U0-V0 ...)
	   GST_VIDEO_FORMAT_RGBx (7) – sparse rgb packed into 32 bit, space last
	   GST_VIDEO_FORMAT_BGRx (8) – sparse reverse rgb packed into 32 bit, space last
	   GST_VIDEO_FORMAT_xRGB (9) – sparse rgb packed into 32 bit, space first
	   GST_VIDEO_FORMAT_xBGR (10) – sparse reverse rgb packed into 32 bit, space first
	   GST_VIDEO_FORMAT_RGBA (11) – rgb with alpha channel last
	   GST_VIDEO_FORMAT_BGRA (12) – reverse rgb with alpha channel last
	   GST_VIDEO_FORMAT_ARGB (13) – rgb with alpha channel first
	   GST_VIDEO_FORMAT_ABGR (14) – reverse rgb with alpha channel first
	   GST_VIDEO_FORMAT_RGB (15) – RGB packed into 24 bits without padding (R-G-B-R-G-B)
	   GST_VIDEO_FORMAT_BGR (16) – reverse RGB packed into 24 bits without padding (B-G-R-B-G-R)
	   GST_VIDEO_FORMAT_Y41B (17) – planar 4:1:1 YUV
	   GST_VIDEO_FORMAT_Y42B (18) – planar 4:2:2 YUV
	   GST_VIDEO_FORMAT_YVYU (19) – packed 4:2:2 YUV (Y0-V0-Y1-U0 Y2-V2-Y3-U2 Y4 ...)
	   GST_VIDEO_FORMAT_Y444 (20) – planar 4:4:4 YUV
	   GST_VIDEO_FORMAT_v210 (21) – packed 4:2:2 10-bit YUV, complex format
	   GST_VIDEO_FORMAT_v216 (22) – packed 4:2:2 16-bit YUV, Y0-U0-Y1-V1 order
	   GST_VIDEO_FORMAT_NV12 (23) – planar 4:2:0 YUV with interleaved UV plane
	   GST_VIDEO_FORMAT_NV21 (24) – planar 4:2:0 YUV with interleaved VU plane
	   GST_VIDEO_FORMAT_GRAY8 (25) – 8-bit grayscale
	   GST_VIDEO_FORMAT_GRAY16_BE (26) – 16-bit grayscale, most significant byte first
	   GST_VIDEO_FORMAT_GRAY16_LE (27) – 16-bit grayscale, least significant byte first
	   GST_VIDEO_FORMAT_v308 (28) – packed 4:4:4 YUV (Y-U-V ...)
	   GST_VIDEO_FORMAT_RGB16 (29) – rgb 5-6-5 bits per component
	   GST_VIDEO_FORMAT_BGR16 (30) – reverse rgb 5-6-5 bits per component
	   GST_VIDEO_FORMAT_RGB15 (31) – rgb 5-5-5 bits per component
	   GST_VIDEO_FORMAT_BGR15 (32) – reverse rgb 5-5-5 bits per component
	   GST_VIDEO_FORMAT_UYVP (33) – packed 10-bit 4:2:2 YUV (U0-Y0-V0-Y1 U2-Y2-V2-Y3 U4 ...)
	   GST_VIDEO_FORMAT_A420 (34) – planar 4:4:2:0 AYUV
	   GST_VIDEO_FORMAT_RGB8P (35) – 8-bit paletted RGB
	   GST_VIDEO_FORMAT_YUV9 (36) – planar 4:1:0 YUV
	   GST_VIDEO_FORMAT_YVU9 (37) – planar 4:1:0 YUV (like YUV9 but UV planes swapped)
	   GST_VIDEO_FORMAT_IYU1 (38) – packed 4:1:1 YUV (Cb-Y0-Y1-Cr-Y2-Y3 ...)
	   GST_VIDEO_FORMAT_ARGB64 (39) – rgb with alpha channel first, 16 bits (native endianness) per channel
	   GST_VIDEO_FORMAT_AYUV64 (40) – packed 4:4:4 YUV with alpha channel, 16 bits (native endianness) per channel (A0-Y0-U0-V0 ...)
	   GST_VIDEO_FORMAT_r210 (41) – packed 4:4:4 RGB, 10 bits per channel
	   GST_VIDEO_FORMAT_I420_10BE (42) – planar 4:2:0 YUV, 10 bits per channel
	   GST_VIDEO_FORMAT_I420_10LE (43) – planar 4:2:0 YUV, 10 bits per channel
	   GST_VIDEO_FORMAT_I422_10BE (44) – planar 4:2:2 YUV, 10 bits per channel
	   GST_VIDEO_FORMAT_I422_10LE (45) – planar 4:2:2 YUV, 10 bits per channel
	   GST_VIDEO_FORMAT_Y444_10BE (46) – planar 4:4:4 YUV, 10 bits per channel (Since: 1.2)
	   GST_VIDEO_FORMAT_Y444_10LE (47) – planar 4:4:4 YUV, 10 bits per channel (Since: 1.2)
	   GST_VIDEO_FORMAT_GBR (48) – planar 4:4:4 RGB, 8 bits per channel (Since: 1.2)
	   GST_VIDEO_FORMAT_GBR_10BE (49) – planar 4:4:4 RGB, 10 bits per channel (Since: 1.2)
	   GST_VIDEO_FORMAT_GBR_10LE (50) – planar 4:4:4 RGB, 10 bits per channel (Since: 1.2)
	   GST_VIDEO_FORMAT_NV16 (51) – planar 4:2:2 YUV with interleaved UV plane (Since: 1.2)
	   GST_VIDEO_FORMAT_NV24 (52) – planar 4:4:4 YUV with interleaved UV plane (Since: 1.2)
	   GST_VIDEO_FORMAT_NV12_64Z32 (53) – NV12 with 64x32 tiling in zigzag pattern (Since: 1.4)
	   GST_VIDEO_FORMAT_A420_10BE (54) – planar 4:4:2:0 YUV, 10 bits per channel (Since: 1.6)
	   GST_VIDEO_FORMAT_A420_10LE (55) – planar 4:4:2:0 YUV, 10 bits per channel (Since: 1.6)
	   GST_VIDEO_FORMAT_A422_10BE (56) – planar 4:4:2:2 YUV, 10 bits per channel (Since: 1.6)
	   GST_VIDEO_FORMAT_A422_10LE (57) – planar 4:4:2:2 YUV, 10 bits per channel (Since: 1.6)
	   GST_VIDEO_FORMAT_A444_10BE (58) – planar 4:4:4:4 YUV, 10 bits per channel (Since: 1.6)
	   GST_VIDEO_FORMAT_A444_10LE (59) – planar 4:4:4:4 YUV, 10 bits per channel (Since: 1.6)
	   GST_VIDEO_FORMAT_NV61 (60) – planar 4:2:2 YUV with interleaved VU plane (Since: 1.6)
	   GST_VIDEO_FORMAT_P010_10BE (61) – planar 4:2:0 YUV with interleaved UV plane, 10 bits per channel (Since: 1.10)
	   GST_VIDEO_FORMAT_P010_10LE (62) – planar 4:2:0 YUV with interleaved UV plane, 10 bits per channel (Since: 1.10)
	   GST_VIDEO_FORMAT_IYU2 (63) – packed 4:4:4 YUV (U-Y-V ...) (Since: 1.10)
	   GST_VIDEO_FORMAT_VYUY (64) – packed 4:2:2 YUV (V0-Y0-U0-Y1 V2-Y2-U2-Y3 V4 ...)
	   GST_VIDEO_FORMAT_GBRA (65) – planar 4:4:4:4 ARGB, 8 bits per channel (Since: 1.12)
	   GST_VIDEO_FORMAT_GBRA_10BE (66) – planar 4:4:4:4 ARGB, 10 bits per channel (Since: 1.12)
	   GST_VIDEO_FORMAT_GBRA_10LE (67) – planar 4:4:4:4 ARGB, 10 bits per channel (Since: 1.12)
	   GST_VIDEO_FORMAT_GBR_12BE (68) – planar 4:4:4 RGB, 12 bits per channel (Since: 1.12)
	   GST_VIDEO_FORMAT_GBR_12LE (69) – planar 4:4:4 RGB, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_GBRA_12BE (70) – planar 4:4:4:4 ARGB, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_GBRA_12LE (71) – planar 4:4:4:4 ARGB, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_I420_12BE (72) – planar 4:2:0 YUV, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_I420_12LE (73) – planar 4:2:0 YUV, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_I422_12BE (74) – planar 4:2:2 YUV, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_I422_12LE (75) – planar 4:2:2 YUV, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_Y444_12BE (76) – planar 4:4:4 YUV, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_Y444_12LE (77) – planar 4:4:4 YUV, 12 bits per channel (Since: 1.12)
	GST_VIDEO_FORMAT_GRAY10_LE32 (78) – 10-bit grayscale, packed into 32bit words (2 bits padding) (Since: 1.14)
	GST_VIDEO_FORMAT_NV12_10LE32 (79) – 10-bit variant of GST_VIDEO_FORMAT_NV12, packed into 32bit words (MSB 2 bits padding) (Since: 1.14)
	GST_VIDEO_FORMAT_NV16_10LE32 (80) – 10-bit variant of GST_VIDEO_FORMAT_NV16, packed into 32bit words (MSB 2 bits padding) (Since: 1.14)
	GST_VIDEO_FORMAT_NV12_10LE40 (81) – Fully packed variant of NV12_10LE32 (Since: 1.16)
	GST_VIDEO_FORMAT_Y210 (82) – packed 4:2:2 YUV, 10 bits per channel (Since: 1.16)
	GST_VIDEO_FORMAT_Y410 (83) – packed 4:4:4 YUV, 10 bits per channel(A-V-Y-U...) (Since: 1.16)
	GST_VIDEO_FORMAT_VUYA (84) – packed 4:4:4 YUV with alpha channel (V0-U0-Y0-A0...) (Since: 1.16)
	GST_VIDEO_FORMAT_BGR10A2_LE (85) – packed 4:4:4 RGB with alpha channel(B-G-R-A), 10 bits for R/G/B channel and MSB 2 bits for alpha channel (Since: 1.16)
	GST_VIDEO_FORMAT_RGB10A2_LE (86) – packed 4:4:4 RGB with alpha channel(R-G-B-A), 10 bits for R/G/B channel and MSB 2 bits for alpha channel (Since: 1.18)
	GST_VIDEO_FORMAT_Y444_16BE (87) – planar 4:4:4 YUV, 16 bits per channel (Since: 1.18)
	GST_VIDEO_FORMAT_Y444_16LE (88) – planar 4:4:4 YUV, 16 bits per channel (Since: 1.18)
	GST_VIDEO_FORMAT_P016_BE (89) – planar 4:2:0 YUV with interleaved UV plane, 16 bits per channel (Since: 1.18)
	GST_VIDEO_FORMAT_P016_LE (90) – planar 4:2:0 YUV with interleaved UV plane, 16 bits per channel (Since: 1.18)
	GST_VIDEO_FORMAT_P012_BE (91) – planar 4:2:0 YUV with interleaved UV plane, 12 bits per channel (Since: 1.18)
	GST_VIDEO_FORMAT_P012_LE (92) – planar 4:2:0 YUV with interleaved UV plane, 12 bits per channel (Since: 1.18)
	GST_VIDEO_FORMAT_Y212_BE (93) – packed 4:2:2 YUV, 12 bits per channel (Y-U-Y-V) (Since: 1.18)
	GST_VIDEO_FORMAT_Y212_LE (94) – packed 4:2:2 YUV, 12 bits per channel (Y-U-Y-V) (Since: 1.18)
	GST_VIDEO_FORMAT_Y412_BE (95) – packed 4:4:4:4 YUV, 12 bits per channel(U-Y-V-A...) (Since: 1.18)
	GST_VIDEO_FORMAT_Y412_LE (96) – packed 4:4:4:4 YUV, 12 bits per channel(U-Y-V-A...) (Since: 1.18)
	GST_VIDEO_FORMAT_NV12_4L4 (97) – NV12 with 4x4 tiles in linear order. (Since: 1.18)
	GST_VIDEO_FORMAT_NV12_32L32 (98) – NV12 with 32x32 tiles in linear order. (Since: 1.18)
	GST_VIDEO_FORMAT_RGBP (99) – Planar 4:4:4 RGB, R-G-B order (Since: 1.20)
	GST_VIDEO_FORMAT_BGRP (100) – Planar 4:4:4 RGB, B-G-R order (Since: 1.20)
	GST_VIDEO_FORMAT_AV12 (101) – Planar 4:2:0 YUV with interleaved UV plane with alpha as 3rd plane. (Since: 1.20)
	GST_VIDEO_FORMAT_ARGB64_LE (102) – RGB with alpha channel first, 16 bits (little endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_ARGB64_BE (103) – RGB with alpha channel first, 16 bits (big endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_RGBA64_LE (104) – RGB with alpha channel last, 16 bits (little endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_RGBA64_BE (105) – RGB with alpha channel last, 16 bits (big endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_BGRA64_LE (106) – Reverse RGB with alpha channel last, 16 bits (little endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_BGRA64_BE (107) – Reverse RGB with alpha channel last, 16 bits (big endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_ABGR64_LE (108) – Reverse RGB with alpha channel first, 16 bits (little endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_ABGR64_BE (109) – Reverse RGB with alpha channel first, 16 bits (big endian) per channel. (Since: 1.20)
	GST_VIDEO_FORMAT_NV12_16L32S (110) – NV12 with 16x32 Y tiles and 16x16 UV tiles. (Since: 1.22)
	GST_VIDEO_FORMAT_NV12_8L128 (111) – NV12 with 8x128 tiles in linear order. (Since: 1.22)
GST_VIDEO_FORMAT_NV12_10BE_8L128 (112) – NV12 10bit big endian with 8x128 tiles in linear order. (Since: 1.22)
	GST_VIDEO_FORMAT_NV12_10LE40_4L4 (113) – GST_VIDEO_FORMAT_NV12_10LE40 with 4x4 pixels tiles (5 bytes per tile row). 
	GST_VIDEO_FORMAT_DMA_DRM (114) – GST_VIDEO_FORMAT_DMA_DRM represent the DMA DRM special format.
	GST_VIDEO_FORMAT_MT2110T (115) – Mediatek 10bit NV12 little endian with 16x32 tiles in linear order, tile 2 bits. (Since: 1.24)
	GST_VIDEO_FORMAT_MT2110R (116) – Mediatek 10bit NV12 little endian with 16x32 tiles in linear order, raster 2 bits. (Since: 1.24)
	GST_VIDEO_FORMAT_A422 (117) – planar 4:4:2:2 YUV, 8 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A444 (118) – planar 4:4:4:4 YUV, 8 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A444_12LE (119) – planar 4:4:4:4 YUV, 12 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A444_12BE (120) – planar 4:4:4:4 YUV, 12 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A422_12LE (121) – planar 4:4:2:2 YUV, 12 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A422_12BE (122) – planar 4:4:2:2 YUV, 12 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A420_12LE (123) – planar 4:4:2:0 YUV, 12 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A420_12BE (124) – planar 4:4:2:0 YUV, 12 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A444_16LE (125) – planar 4:4:4:4 YUV, 16 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A444_16BE (126) – planar 4:4:4:4 YUV, 16 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A422_16LE (127) – planar 4:4:2:2 YUV, 16 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A422_16BE (128) – planar 4:4:2:2 YUV, 16 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A420_16LE (129) – planar 4:4:2:0 YUV, 16 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_A420_16BE (130) – planar 4:4:2:0 YUV, 16 bits per channel (Since: 1.24)
	GST_VIDEO_FORMAT_GBR_16LE (131) – planar 4:4:4 RGB, 16 bits per channel (Since: 1.24)
GST_VIDEO_FORMAT_GBR_16BE (132) – planar 4:4:4 RGB, 16 bits per channel (Since: 1.24) 
	*/
