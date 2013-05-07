#ifndef CUSTOM_COMM_TYPE_H_
#define CUSTOM_COMM_TYPE_H_

#include <pipeline/pf_export.hpp>

/*
  jhtark. for custom player
*/
typedef struct
{
  /* to support the seamless play on MTK platform - SSPK, Netflix, HLS. */
  guint8 bSeamlessPlay;          // from CP      // default should be FALSE
  guint8 bPlaybackDiag;          // information including the following: chunk request / end / index / url / sysTime
  guint8 breserved1;             // for memory align
  guint8 breserved2;             // for memory align
  guint32 maxWidth;              // from CP
  guint32 maxHeight;             // from CP
  guint32 start_bps;
  guint32 max_bps;
  guint32 min_bps;
  guint32 diagBatchSize;         // size which will be posted th the server at once.
  char postServerURL[2048];     // Server URL which can receive the diagnostics.
} MEDIA_VIDEO_ADAPTIVE_INFO_T;

/**
 * 3D Types for Media play
 */
typedef enum MEDIA_3D_TYPES
{
  MEDIA_3D_NONE = 0x00,

  //added, interim format - half
  MEDIA_3D_SIDE_BY_SIDE_HALF = 0x01,
  MEDIA_3D_SIDE_BY_SIDE_HALF_LR = MEDIA_3D_SIDE_BY_SIDE_HALF,

  MEDIA_3D_SIDE_BY_SIDE_HALF_RL = 0x02,
  MEDIA_3D_TOP_AND_BOTTOM_HALF = 0x03,
  MEDIA_3D_BOTTOM_AND_TOP_HALF = 0x04,
  MEDIA_3D_CHECK_BOARD = 0x05,                          /**< for T/B, S/S, Checker, Frame Seq*/
  MEDIA_3D_FRAME_SEQUENTIAL = 0x06,                     /**< for T/B, S/S, Checker, Frame Seq*/
  MEDIA_3D_COLUMN_INTERLEAVE = 0x07,                    /**< for H.264*/

  //added, Full format
  MEDIA_3D_SIDE_BY_SIDE_LR = 0x08,
  MEDIA_3D_SIDE_BY_SIDE_RL = 0x09,
  MEDIA_3D_FRAME_PACKING = 0x0A,                        /**< Full format*/
  MEDIA_3D_FIELD_ALTERNATIVE = 0x0B,                    /**< Full format*/
  MEDIA_3D_LINE_ALTERNATIVE = 0x0C,                     /**< Full format*/
  MEDIA_3D_DUAL_STREAM = 0x0D,                          /**< Full format*/
  MEDIA_3D_2DTO3D = 0x0E,                                       /**< Full format*/

} MEDIA_3D_TYPES_T;


typedef struct
{
  gint wmvVer;
  guint32 wmvFourcc;

  /* to support raw video es(without header) */
  guint8 *codec_data;
  gint codec_data_size;

} MEDIA_VIDEO_WMV_INFO_T;

typedef struct
{
  /* to support raw video es(without header) */
  guint8 *codec_data;
  gint codec_data_size;

} MEDIA_VIDEO_H264_INFO_T;

/**
 * Transport Type
 */
typedef enum
{
  MEDIA_CUSTOM_CONTAINER_UNKNOWN,
  MEDIA_CUSTOM_CONTAINER_ES,
  MEDIA_CUSTOM_CONTAINER_TS,
  MEDIA_CUSTOM_CONTAINER_MPEG2PES,
  MEDIA_CUSTOM_CONTAINER_VOB,
  MEDIA_CUSTOM_CONTAINER_MPEG1PS,
  MEDIA_CUSTOM_CONTAINER_DSSES,
  MEDIA_CUSTOM_CONTAINER_DSSPES,
  MEDIA_CUSTOM_CONTAINER_ASF,
  MEDIA_CUSTOM_CONTAINER_AVI,
  MEDIA_CUSTOM_CONTAINER_MP4,
  MEDIA_CUSTOM_CONTAINER_FLV,
  MEDIA_CUSTOM_CONTAINER_MKV,
  MEDIA_CUSTOM_CONTAINER_WAV,
  MEDIA_CUSTOM_CONTAINER_WMV,
  MEDIA_CUSTOM_CONTAINER_MP3,
  MEDIA_CUSTOM_CONTAINER_MCAST_TS,
  MEDIA_CUSTOM_CONTAINER_MAX
} MEDIA_CUSTOM_CONTAINER_TYPE_T;        // EME_BB_TRANSPORT_TYPE_T

/**
 * Flash ES Channel Info
 */
typedef enum
{
  MEDIA_DATA_CH_NONE = 0,
  MEDIA_DATA_CH_A,                                                              /** For H264 data */
  MEDIA_DATA_CH_B,                                                              /** For AAC data */
  MEDIA_DATA_CH_MAX = MEDIA_DATA_CH_B,

} MEDIA_DATA_CHANNEL_T;

typedef struct MEDIA_VIDEO_DATA_INFO
{
  guint vcodec;

  guint bufferMinLevel;
  guint bufferMaxLevel;
  guint prebufferLevel;
  guint userQBufferLevel;       // set Queue buffer size

  union
  {
    MEDIA_VIDEO_WMV_INFO_T wmvInfo;
    MEDIA_VIDEO_H264_INFO_T h264Info;
  } video_data;

  /* to support sspk video frame gathering(for L9) */
  gboolean isSSPK;

  /* to support seamless playback */
  MEDIA_VIDEO_ADAPTIVE_INFO_T adaptiveInfo;

  /* to support VUDU 3D (without SEI case) */
  MEDIA_3D_TYPES_T interleaving_type;   // from CP

  /* to support raw video es(without header) */
  guint8 *codec_data;           // from CP
  gint codec_data_size;         // from CP
} MEDIA_VIDEO_DATA_INFO_T;


typedef enum ADEC_CH
{
  ADEC_CH_MAIN = 0,
  ADEC_CH_SUB
} ADEC_CH_T;

typedef struct MEDIA_AUDIO_DATA_INFO
{
  // TODO: codec 별 data type define
  guint acodec;                 // audio codec  // from CP

  guint bufferMinLevel;         // from CP
  guint bufferMaxLevel;         // from CP
  guint prebufferLevel;         // from CP
  guint userQBufferLevel;       // set Queue buffer siz
  gboolean bBufferedPlay;

  union
  {
    MEDIA_AUDIO_AAC_INFO_T aacInfo;
    MEDIA_AUDIO_WMA_INFO_T wmaInfo;
    MEDIA_AUDIO_PCM_INFO_T pcmInfo;
  } audio_data;

  ADEC_CH_T adecCh;

} MEDIA_AUDIO_DATA_INFO_T;

typedef  enum    // static header 로 이동
{
  MEDIA_ADEC_DECODE_MAIN    = 0x01,        ///< Main Audio Decoding
  MEDIA_ADEC_DECODE_ASSO    = 0x02,        ///< Associate Audio Decoding

  MEDIA_ADEC_MIX_BUF0        = 0x10,        ///< PCM Audio Mix in PCM Buffer0(Main PCM Audio Present)
  MEDIA_ADEC_MIX_BUF1        = 0x20,        ///< PCM Audio Mix in PCM Buffer1(Associate PCM Audio Present)
  MEDIA_ADEC_MIX_BUF2        = 0x40,        ///< PCM Audio Mix in PCM Buffer2
  MEDIA_ADEC_MIX_BUF3        = 0x80        ///< PCM Audio Mix in PCM Buffer3

} MEDIA_ADEC_DEC_MODE_T;

typedef struct MEDIA_CUSTOM_CONTENT_INFO
{
  MEDIA_TRANSPORT_T mediaTransportType;
  MEDIA_CUSTOM_CONTAINER_TYPE_T container;
  guint64 size;
  guint32 vcodec;                // video codec
  guint32 acodec;                // audio codec

  MEDIA_DATA_CHANNEL_T esCh;

  gint64 ptsToDecode;           // to support seek
  gboolean pauseAtDecodeTime;
  gboolean bRestartStreaming;    // when underflow is detected - VUDU
  gboolean bSeperatedPTS;        // TRUE : pts is sent by parameter (ES case only)
  gboolean bSecurityVideoPath;   // use SVP path
  gboolean bNeedData;            //to supprot HTML5 YouTube feed (ES case only)

  gint32 preBufferTime;

  gboolean bUseBufferCtrl;       // if it is TRUE, the below time value is meaningful.
  gint32 bufferingMinTime;
  gint32 bufferingMaxTime;
  guint8 bufferMinPercent;
  guint8 bufferMaxPercent;

  MEDIA_VIDEO_DATA_INFO_T videoDataInfo;
  MEDIA_AUDIO_DATA_INFO_T audioDataInfo;

  guint16 inPort;                /* to support wifi display */
  guint32 delayOffset;           /* to support wifi display */

#if 0                           /* jhtark - webos refactory - temp [ 변수 정리 필요 ] */
#ifdef INCLUDE_DRM_MODULE
  MEDIA_DRM_TYPE drmType;
#else
  DRM_TYPE drmType;
#endif

  HOA_DLNA_INFO_T dlnaInfo;     /* DLNA info (from server to client) */
  gboolean isDLNA;               /* DLNA transport type */
  gboolean isDLNA_notSeekableContent;    /* check for DLNA contents seekable */
#endif

  NEDIA_MVPD_INFO_T mvpdOpt;


  guint32 start_bps;
  guint32 thumbnailOnPause_Width;                /**< thumbnail size define. for thumbnail on pause status func. */
  guint32 thumbnailOnPause_Hight;                /**< thumbnail size define. for thumbnail on pause status func. */
  gboolean bVdecChannelReserved;               /**< for external VDEC resource info (from DVR) */
  gboolean bIsTTSEngine;                 /**< for setting H13 adec property> */
  gboolean bIsNetflix;                   /**< for netflix specific case> */

} MEDIA_CUSTOM_CONTENT_INFO_T;

/**
 * MEDIA Static Src type
 */
typedef enum
{
  MEDIA_CUSTOM_SRC_TYPE_URI = 0,        // for test
  MEDIA_CUSTOM_SRC_TYPE_FILE,   // for test
  MEDIA_CUSTOM_SRC_TYPE_SCREEN_SHARE,   // for wifi display
  MEDIA_CUSTOM_SRC_TYPE_HLS,    // for Http live streaming
  MEDIA_CUSTOM_SRC_TYPE_FCC,
  MEDIA_CUSTOM_SRC_TYPE_RTP_MCAST,
  MEDIA_CUSTOM_SRC_TYPE_MPEG_DASH,
  MEDIA_CUSTOM_SRC_TYPE_ES,     // for PUSH after parsing content (raw format)  (ex) flash es, smooth streaming, netflix
  MEDIA_CUSTOM_SRC_TYPE_PUSH,   // for PUSH before parsing content (with container) (ex)VUDU, Widevine
  MEDIA_CUSTOM_SRC_TYPE_DOWNLOAD,       // for download content to file (ex) Progressive download, Subtitle
  MEDIA_CUSTOM_SRC_TYPE_TTS,    //for PUSH before parsing content (with container) (ex)JP marlin
  MEDIA_CUSTOM_SRC_TYPE_MAX = MEDIA_CUSTOM_SRC_TYPE_DOWNLOAD,
} MEDIA_CUSTOM_SRC_TYPE_T;      /* jhtark webos refactory -LMF_MEDIA_SRC_TYPE_T -> MEDIA_CUSTOM_SRC_TYPE_T 변경 */


#define MAX_TRACK_NUM 8
#define MEDIA_BUFFER_SIZE (24*1024*1024)

/**
* This enumeration describes the src elem idx for static player
*
*/
typedef enum
{
  IDX_MULTI = 0,
  IDX_VIDEO = IDX_MULTI,
  IDX_AUDIO,
  IDX_MAX,
} MEDIA_SRC_ELEM_IDX_T;         /* jhtark webos refactory -LMF_SRC_ELEM_IDX_T -> MEDIA_SRC_ELEM_IDX_T 변경 */

#endif
// end of file
