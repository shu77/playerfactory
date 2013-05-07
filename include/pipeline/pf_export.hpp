
/** @file pf_export.h
 *
 *  export types for pipeline factory.
 *
 *  @author MS part (mf-dev@lge.com)
*/
#ifndef _PF_EXPORT_C_H_
#define _PF_EXPORT_C_H_

#include <glib.h>

/**
 * Callback Message of Media play
 */
typedef enum MEDIA_CB_MSG
{
  MEDIA_CB_MSG_NONE = 0x00,                                 /**< message ���� ���� */
  MEDIA_CB_MSG_PLAYEND,                                  /**< media data�� ���̾ ����� ����� */
  MEDIA_CB_MSG_PLAYSTART,                                  /**< media�� ������ ��� ���۵� */

  MEDIA_CB_MEDIAFRAMEWORK_END,  /* PM�� mediaframework �� �����ϴ� ��� */

  MEDIA_CB_MSG_SOURCE_INFO_UPDATED,                      /**< tag list�� audio codec ������ �ö�ͼ� source info get�� �غ�� */
  MEDIA_CB_MSG_NOT_SUPPORTED_AUDIO_CODEC,                  /**< mediapipeline�̳� demuxer���� ������� ���ϴ� audio codec���� �Ǵ��� */

  MEDIA_CB_MSG_BUFFERING_START,                          /**< buffering start */
  MEDIA_CB_MSG_NOT_SUPPORTED_VIDEO_CODEC,                  /**< mediapipeline�̳� demuxer���� ������� ���ϴ� video codec���� �Ǵ��� */
  MEDIA_CB_MSG_YOUTUBE_NEED_ADATA,                      /**< HTML5 youtube playback test */
  MEDIA_CB_MSG_YOUTUBE_NEED_VDATA,                      /**< HTML5 youtube playback test */
  MEDIA_CB_MSG_3DTVFORMAT_DUAL_STREAM,                  /**< 3D Format Dual Stream */
  MEDIA_CB_MSG_3Dto2D_STREAM,                              /**< 3D to 2D Stream */

  MEDIA_CB_MSG_RESERVED1,
  MEDIA_CB_MSG_RESERVED2,
  MEDIA_CB_MSG_RESERVED3,

  // for specific applications.
  MEDIA_CB_MSG_SPECIAL_START = 0x200,
  MEDIA_CB_MSG_CONNECTED,                                  /**< html 5 */
  MEDIA_CB_MSG_LOADED_METADATA,                          /**< html 5 */
  MEDIA_CB_MSG_SEEK_DONE,                                  /**< SmartShare */
  MEDIA_CB_MSG_BUFFERING_PERCENT_0 = 0x300,             /**< China CP */
  MEDIA_CB_MSG_BUFFERING_PERCENT_10,
  MEDIA_CB_MSG_BUFFERING_PERCENT_20,
  MEDIA_CB_MSG_BUFFERING_PERCENT_30,
  MEDIA_CB_MSG_BUFFERING_PERCENT_40,
  MEDIA_CB_MSG_BUFFERING_PERCENT_50,
  MEDIA_CB_MSG_BUFFERING_PERCENT_60,
  MEDIA_CB_MSG_BUFFERING_PERCENT_70,
  MEDIA_CB_MSG_BUFFERING_PERCENT_80,
  MEDIA_CB_MSG_BUFFERING_PERCENT_90,
  MEDIA_CB_MSG_BUFFERING_PERCENT_100,
  MEDIA_CB_MSG_SPECIAL_END = 0x4FF,

  // for internal use.
  MEDIA_CB_MSG_STOPPED = 0x500,                          /**< SMH ����, Thumbnail ����� */
  MEDIA_CB_MSG_PAUSE_DONE,                               /**< SMH ���� ����� */
  MEDIA_CB_MSG_RESUME_DONE,                               /**< for WIDEVINE */
  MEDIA_CB_MSG_THUMBNAIL_EXTRACTED,                       /**< Thumbnail ����� */
  MEDIA_CB_MSG_THUMBNAIL_ERROR,                           /**< Thumbnail ���� ���� ���� **/
  MEDIA_CB_MSG_THUMBNAIL_TIMEOUT,                       /**< Thumbnail ���� ���� timeout **/


  // old error msg //
  MEDIA_CB_MSG_ERR_PLAYING = 0xf000,                     /**< ����� error �߻� */
  MEDIA_CB_MSG_ERR_BUFFERFULL,                          /**< ����� buffer full �߻� */
  MEDIA_CB_MSG_ERR_BUFFERLOW,                              /**< ����� buffer low �߻� */
  MEDIA_CB_MSG_ERR_NOT_FOUND,                              /**< ����Ϸ��� pathȤ�� url���� ���� �߰ߵ��� ���� */
  MEDIA_CB_MSG_ERR_CODEC_NOT_SUPPORTED,                  /**< html 5 */
  MEDIA_CB_MSG_ERR_BUFFER_20MS,                          /**< ����� buffer data 20msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_40MS,                          /**< ����� buffer data 40msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_60MS,                          /**< ����� buffer data 60msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_80MS,                          /**< ����� buffer data 80msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_100MS,                          /**< ����� buffer data 100msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_120MS,                          /**< ����� buffer data 120msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_140MS,                          /**< ����� buffer data 140msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_160MS,                          /**< ����� buffer data 160msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_180MS,                          /**< ����� buffer data 180msec ���� */
  MEDIA_CB_MSG_ERR_BUFFER_200MS,                          /**< ����� buffer data 200msec ���� */
  MEDIA_CB_MSG_ERR_AUDIO_DECODING_FAILED,                  /**< ��� �� audio decoding error �߻� (�߸��� ��Ʈ��), ��� ���������� ���� */
  // end old error msg //
  /* gstrreamer core error msg */
  MEDIA_CB_MSG_START_GST_MSG = 0xf100,  //d16
  MEDIA_CB_MSG__GST_CORE_ERROR_FAILED,  // a general error which doesn't fit in any other category. Make sure you add a custom message to the error call.
  MEDIA_CB_MSG__GST_CORE_ERROR_TOO_LAZY,        // do not use this except as a placeholder for deciding where to go while developing code.
  MEDIA_CB_MSG__GST_CORE_ERROR_NOT_IMPLEMENTED, // use this when you do not want to implement this functionality yet.
  MEDIA_CB_MSG__GST_CORE_ERROR_STATE_CHANGE,    // used for state change errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_PAD,     // used for pad-related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_THREAD,  // used for thread-related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_NEGOTIATION,     // used for negotiation-related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_EVENT,   //  used for event-related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_SEEK,    // used for seek-related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_CAPS,    //  used for caps-related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_TAG,     //  used for negotiation-related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_MISSING_PLUGIN,  // used if a plugin is missing.
  MEDIA_CB_MSG__GST_CORE_ERROR_CLOCK,   // used for clock related errors.
  MEDIA_CB_MSG__GST_CORE_ERROR_DISABLED,        //d30 // used if functionality has been disabled at compile time (Since: 0.10.13).
  /* gstreamer library error msg */
  MEDIA_CB_MSG__GST_LIBRARY_ERROR_FAILED,       // a general error which doesn't fit in any other category. Make sure you add a custom message to the error call.
  MEDIA_CB_MSG__GST_LIBRARY_ERROR_TOO_LAZY,     // do not use this except as a placeholder for deciding where to go while developing code.
  MEDIA_CB_MSG__GST_LIBRARY_ERROR_INIT, // used when the library could not be opened.
  MEDIA_CB_MSG__GST_LIBRARY_ERROR_SHUTDOWN,     // used when the library could not be closed.
  MEDIA_CB_MSG__GST_LIBRARY_ERROR_SETTINGS,     // used when the library doesn't accept settings.
  MEDIA_CB_MSG__GST_LIBRARY_ERROR_ENCODE,       // used when the library generated an encoding error.
  /* gstreamer resource error msg */
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_FAILED,      // a general error which doesn't fit in any other category. Make sure you add a custom message to the error call.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_TOO_LAZY,    // do not use this except as a placeholder for deciding where to go while developing code.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_NOT_FOUND,   // used when the resource could not be found.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_BUSY,        //d40 // used when resource is busy.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_OPEN_READ,   // used when resource fails to open for reading.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_OPEN_WRITE,  // used when resource fails to open for writing.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_OPEN_READ_WRITE,     // used when resource cannot be opened for both reading and writing, or either (but unspecified which).
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_CLOSE,       // used when the resource can't be closed.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_READ,        // used when the resource can't be read from.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_WRITE,       // used when the resource can't be written to.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_SEEK,        // used when a seek on the resource fails.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_SYNC,        // used when a synchronize on the resource fails.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_SETTINGS,    // used when settings can't be manipulated on.
  MEDIA_CB_MSG__GST_RESOURCE_ERROR_NO_SPACE_LEFT,       //d50 // used when the resource has no space left.
  /* gstreamer stream error msg */
  MEDIA_CB_MSG__GST_STREAM_ERROR_FAILED,        // a general error which doesn't fit in any other category. Make sure you add a custom message to the error call.
  MEDIA_CB_MSG__GST_STREAM_ERROR_TOO_LAZY,      // do not use this except as a placeholder for deciding where to go while developing code.
  MEDIA_CB_MSG__GST_STREAM_ERROR_NOT_IMPLEMENTED,       // use this when you do not want to implement this functionality yet.
  MEDIA_CB_MSG__GST_STREAM_ERROR_TYPE_NOT_FOUND,        // used when the element doesn't know the stream's type.
  MEDIA_CB_MSG__GST_STREAM_ERROR_WRONG_TYPE,    // used when the element doesn't handle this type of stream.
  MEDIA_CB_MSG__GST_STREAM_ERROR_CODEC_NOT_FOUND,       // used when there's no codec to handle the stream's type.
  MEDIA_CB_MSG__GST_STREAM_ERROR_DECODE,        // used when decoding fails.
  MEDIA_CB_MSG__GST_STREAM_ERROR_ENCODE,        // used when encoding fails.
  MEDIA_CB_MSG__GST_STREAM_ERROR_DEMUX, // used when demuxing fails.
  MEDIA_CB_MSG__GST_STREAM_ERROR_MUX,   // used when muxing fails.
  MEDIA_CB_MSG__GST_STREAM_ERROR_FORMAT,        // used when the stream is of the wrong format (for example, wrong caps).
  MEDIA_CB_MSG__GST_STREAM_ERROR_DECRYPT,       // used when the stream is encrypted and can't be decrypted because this is not supported by the element. (Since: 0.10.20)
  MEDIA_CB_MSG__GST_STREAM_ERROR_DECRYPT_NOKEY, // used when the stream is encrypted and can't be decrypted because no suitable key is available. (Since: 0.10.20)
  MEDIA_CB_MSG_END_GST_MSG,

  MEDIA_CB_MSG_ERR_NET_DISCONNECTED = 0xff00,             /**< network�� ���� */
  MEDIA_CB_MSG_ERR_NET_BUSY,                              /**< network�� ����� */
  MEDIA_CB_MSG_ERR_NET_CANNOT_PROCESS,                  /**< ��Ÿ ������ network�� ��� �Ұ��� */
  MEDIA_CB_MSG_ERR_NET_CANNOT_CONNECT,                  /**< html 5 */
  MEDIA_CB_MSG_ERR_NET_SLOW,                              /**< network�� ���� */

  MEDIA_CB_MSG_ERR_WMDRM_CANNOT_PROCESS = 0xff10,         /**< WMDRM license error. */
  MEDIA_CB_MSG_ERR_WMDRM_LIC_FAILLOCAL,                  /**< WMDRM license error. ����� license�� ����. */
  MEDIA_CB_MSG_ERR_WMDRM_LIC_FAILTRANSFER,              /**< WMDRM license error. license ������ error & ���۵� license error. */
  MEDIA_CB_MSG_ERR_WMDRM_LIC_EXPIRED,                      /**< WMDRM license error. ����� License�� �����. */
  MEDIA_CB_MSG_REQ_ONLY_PLAY_AGAIN,                      /**< Live streaming ��, network ���� ���� ������ media play��õ��� ��û��.  */
  MEDIA_CB_MSG_ERR_VERIMATRIX_DRM_FAILED,                  /**< Verimatrix DRM ��� API ����� error message ó���� ���� �߰�. */

  MEDIA_CB_MSG_ERR_SECUREMEDIA_REQUEST_DENIED,  /**< Securemedia DRM register fail *///1300
  MEDIA_CB_MSG_ERR_SECUREMEDIA_ITEM_NOT_FOUND,  /**< Securemedia DRM Esam client ���� *///1301
  MEDIA_CB_MSG_ERR_SECUREMEDIA_SERVICE_NOT_AVAILABLE,   /**< Securemedia DRM mac �ߺ� ������ service ��û�� ���ѵ� *///1302
  MEDIA_CB_MSG_ERR_SECUREMEDIA_UNDEFINED_ERROR, /**< Securemedia DRM undefined error *///1303

  MEDIA_CB_MSG_ADAPTIVE_BUFFERING_START = 0xff50,       // Adaptive Streaming��
  MEDIA_CB_MSG_ADAPTIVE_BUFFERING_END,
  MEDIA_CB_MSG_ADAPTIVE_FORCED_RESUME,

  MEDIA_CB_MSG_ERR_HLS_302 = 0xff70,                     /**< HLS server error 302 */
  MEDIA_CB_MSG_ERR_HLS_404,                              /**< HLS server error 404 */
  MEDIA_CB_MSG_ERR_HLS_503,                              /**< HLS server error 503 */
  MEDIA_CB_MSG_ERR_HLS_504,                              /**< HLS server error 504 */
  MEDIA_CB_MSG_ERR_HLS_KEYFILE_SIZE_ZERO,               /**< HLS server error keyfile size zero */

  MEDIA_CB_MSG_LAST
} MEDIA_CB_MSG_T;

typedef enum MEDIA_TRANSPORT
{
  /**< from gp4 media types >**/
  MEDIA_TRANS_UNKNOWN = 0x00,
  MEDIA_TRANS_PLAYBIN_START,    /* ---------------------------------------------------------- */
  MEDIA_TRANS_FILE,                  /**< File. File Play�� ����. */
  MEDIA_TRANS_DLNA,                  /**< SmartShare DLNA. File Play�� ����. */
  MEDIA_TRANS_HTTP_DOWNLOAD,          /**< HTTP Progressive download play�� ����. */
  MEDIA_TRANS_URI,                  /**< �⺻ URI play�� ����: local file, streaming ��� ���� */
  MEDIA_TRANS_AD,                    /**< ���� ����� ����: FF & Seek disable�ǰ� ���� ���� ���޿� �Լ� ȣ���. */
  MEDIA_TRANS_PLEX,                  /**< Plex media server URI Play�� ����. */
  MEDIA_TRANS_PLAYBIN_END,      /* ---------------------------------------------------------- */

  MEDIA_TRANS_STATIC_START,     /* ---------------------------------------------------------- */
  MEDIA_TRANS_BUFFERSTREAM,          /**< Stream Play�� ����. */
  MEDIA_TRANS_WIDEVINE,              /**< Widevine Stream Play�� ����. */
  MEDIA_TRANS_ORANGE_VOD,              /**< Orange VoD Play�� ����. */
  MEDIA_TRANS_MSIIS,                  /**< MS Smooth Streaming�� ����. */
  MEDIA_TRANS_SCREEN_SHARE,          /**< SmartShare Wifi Display Play�� ����. */
  MEDIA_TRANS_HLS,                   /**< Http Live Streaming�� ����. */
  MEDIA_TRANS_JPMARLIN,               /**< japan marlin Play�� ����. */
  MEDIA_TRANS_BROADCAST,              /**< FCC. MCast Streaming�� ����. */
  MEDIA_TRANS_MPEG_DASH,              /**< MPEG-DASH format ��� �� ����. */
  MEDIA_TRANS_YOUTUBE_DASH,          /**< youtube�� Ưȭ�� MPEG-DASH Play�� ����. */
  MEDIA_TRANS_MVPD,                  /**< MVPD ��� path */
  MEDIA_TRANS_CAMERA,              /**< Camera Preview ��� path */
  MEDIA_TRANS_GESTURE,               /**< Gesture Preview ��� */
  MEDIA_TRANS_STATIC_END,       /* ---------------------------------------------------------- */
} MEDIA_TRANSPORT_T;

/**
 * This enumeration describes file format(container type) of media.
 *
 */
typedef enum MEDIA_FORMAT
{
  MEDIA_FORMAT_RAW = 0x00,                 /**< File Format�� ���� ����, Audio �Ǵ� Video codec���� encoding �� raw data */
  MEDIA_FORMAT_WAV = 0x01,                 /**< wave file format (Audio). */
  MEDIA_FORMAT_MP3 = 0x02,                 /**< mp3 file format (Audio). */
  MEDIA_FORMAT_AAC = 0x03,                 /**< aac file format (Audio). */
  MEDIA_FORMAT_RA = 0x04,                     /**< ra file format (Audio). */

  MEDIA_FORMAT_AVI = (0x01 << 8),       /**< avi file format (Video). */
  MEDIA_FORMAT_MP4 = (0x02 << 8),       /**< mpeg4 file format (Video). */
  MEDIA_FORMAT_MPEG1 = (0x03 << 8),       /**< mpeg1 file format (Video). */
  MEDIA_FORMAT_MPEG2 = (0x04 << 8),       /**< mpeg2 file format (Video). */
  MEDIA_FORMAT_ASF = (0x05 << 8),       /**< asf file format (Video). */
  MEDIA_FORMAT_MKV = (0x06 << 8),       /**< mkv file format (Video). */
  MEDIA_FORMAT_PS = (0x07 << 8),           /**< ps file format (Video). */
  MEDIA_FORMAT_TS = (0x08 << 8),           /**< ts file format (Video). */
  MEDIA_FORMAT_RM = (0x09 << 8),           /**< rm file format (Video). */

  MEDIA_FORMAT_FLV = (0x0B << 8),       /**< flv file format (Video). */
  MEDIA_FORMAT_F4V = (0x0C << 8),       /**< f4v file format (Video). */
  MEDIA_FORMAT_ISM = (0x0D << 8),       /**< ism file format (Video). */

  MEDIA_FORMAT_AUDIO_MASK = 0xFF,                 /**< Audio file format mask */
  MEDIA_FORMAT_VIDEO_MASK = (0xFF << 8),       /**< Video file format mask */
} MEDIA_FORMAT_T;


/**
 * This enumeration describes video codec of media.
 *
 */
typedef enum MEDIA_CODEC_VIDEO
{
  MEDIA_VIDEO_NONE = 0x0000,                /**< No Video */
  MEDIA_VIDEO_ANY = 0x0000,                 /**< Any Video codec */

  MEDIA_VIDEO_MPEG1 = 0x0100,               /**< mpeg1 codec */
  MEDIA_VIDEO_MPEG2 = 0x0200,               /**< mpeg2 codec */
  MEDIA_VIDEO_MPEG4 = 0x0300,               /**< mpeg4 codec */
  MEDIA_VIDEO_MJPEG = 0x0400,               /**< mjpeg codec */
  MEDIA_VIDEO_H264 = 0x0500,                /**< h.264 codec */
  MEDIA_VIDEO_REALVIDEO = 0x0600,           /**< real video codec */
  MEDIA_VIDEO_WMV = 0x0700,                 /**< wmv codec */
  MEDIA_VIDEO_YUY2 = 0x0800,                /**< YUY2 (YUV422) format */
  MEDIA_VIDEO_VC1 = 0x0900,                 /**< VC1 codec */
  MEDIA_VIDEO_DIVX = 0x0A00,                /**< Divx codec */
  MEDIA_VIDEO_MVC = 0x0B00,                 /**< MVC codec */
  MEDIA_VIDEO_H263 = 0x0C00,    /**< h.263 codec */// ���� �� ���� �ʿ�.
  MEDIA_VIDEO_VP8 = 0x0D00,                 /**< On2 VP8 codec */

  MEDIA_VIDEO_NOT_SUPPORTED = 0xF000,

  MEDIA_VIDEO_MASK = 0xFF00             /**< Video codec mask */
} MEDIA_CODEC_VIDEO_T;

/**
 * This enumeration describes audio codec of media.
 *
 */
typedef enum MEDIA_CODEC_AUDIO
{
  MEDIA_AUDIO_NONE = 0x0000,            /**< No Audio */
  MEDIA_AUDIO_ANY = 0x0000,             /**< Any Audio codec */

  MEDIA_AUDIO_MP3 = 0x0001,             /**< mp3 codec */
  MEDIA_AUDIO_AC3 = 0x0002,             /**< ac3 codec */
  MEDIA_AUDIO_MPEG = 0x0003,            /**< mpeg codec */
  MEDIA_AUDIO_AAC = 0x0004,             /**< aac codec */
  MEDIA_AUDIO_CDDA = 0x0005,    /**< cdda codec *///not implemented in LMF
  MEDIA_AUDIO_PCM = 0x0006,             /**< pcm codec */
  MEDIA_AUDIO_LBR = 0x0007,     /**< lbr codec *///not implemented in LMF
  MEDIA_AUDIO_WMA = 0x0008,             /**< wma codec */
  MEDIA_AUDIO_DTS = 0x0009,             /**< dts codec */
  MEDIA_AUDIO_AC3PLUS = 0x000A,         /**< ac3 plus codec */
  MEDIA_AUDIO_RA = 0x000B,              /**< ra  plus codec */
  MEDIA_AUDIO_AMR = 0x000C,             /**< amr plus codec */
  MEDIA_AUDIO_HEAAC = 0x000D,   //not implemented in LMF (could not tell from AAC)
  MEDIA_AUDIO_PCMWAV = 0x000E,  //not implemented in LMF
  MEDIA_AUDIO_WMA_PRO = 0x000F, //not implemented in LMF
  MEDIA_AUDIO_VORBIS = 0x0010,          /**< vorbis codec */
//  MEDIA_AUDIO_XPCM    = 0x0020,
  MEDIA_AUDIO_NOT_SUPPORTED = 0x00F0,       /** Audio not supported */

  MEDIA_AUDIO_MASK = 0x00FF             /**< Audio codec mask */
} MEDIA_CODEC_AUDIO_T;


/**
 * Type definition of Media Codec.
 * MEDIA_CODEC_T�� MEDIA_CODEC_AUDIO_T, MEDIA_CODEC_VIDEO_T, MEDIA_CODEC_IMAGE_T�� ORing�� ���� ��Ÿ����.
 *
 */
typedef guint32 MEDIA_CODEC_T;

/**
 * Structure of widevine options
 */
typedef struct WIDEVINE_OPTION
{
  gboolean bSeperatedStream;             /**< seperated AV  */
  gfloat playTime;
  gfloat playRate;
  guint32 dummyWord;
} __attribute__ ((packed)) WIDEVINE_OPTION_T;

/**
 * Structure of ES options
 */
typedef struct ES_STREAM_OPT
{
  guint32 audioUserQBufferLevel;       // ES audio qeueu buffer size
  guint32 audioBufferMinLevel;       // ES audio app buffer size
  guint32 audioBufferMaxLevel;       // ES audio app buffer size
  guint32 videoUserQBufferLevel;       // ES video qeueu buffer size
  guint32 videoBufferMinLevel;       // ES video app buffer size
  guint32 videoBufferMaxLevel;       // ES video app buffer size

  gint64 ptsToDecode;
  gboolean pauseAtDecodeTime;
  //removed //gboolean             bIsAudioOnly;
  gboolean bSeperatedPTS;       // check ES chunk. (include PTS header)
  gint64 preBufferByte;         // prebuffering level set. (static pipeline, appsource)
} __attribute__ ((packed)) ES_STREAM_OPT_T;

/**
 * Types for playmode setting: bufferingOnly, playOnly (HTML5 case)
 */
typedef enum MEDIA_PLAYMODE
{
  BUFFERING_AND_PLAY = 0,
  BUFFERING_ONLY,
  PLAY_ONLY
} MEDIA_PLAYMODE_T;

/**
 * This enumeration describes extra stream play option type of media. [NetCast40]
 */
typedef enum MEDIA_EXT_STREAMOPT_TYPE
{
  MEDIA_EXT_STREAMOPT_TYPE_NONE = 0,    /**< No opt */
  MEDIA_EXT_STREAMOPT_TYPE_AAC,         /**< AAC stream info */
  MEDIA_EXT_STREAMOPT_TYPE_WMA,         /**< WMA stream info */
  MEDIA_EXT_STREAMOPT_TYPE_PCM,         /**< PCM stream info */
  MEDIA_EXT_STREAMOPT_TYPE_NUM
} MEDIA_EXT_STREAMOPT_TYPE_T;

/**
 * This enumeration describes the audio sampling rate.
 *
 */
typedef enum MEDIA_AUDIO_SAMPLERATE
{
  MEDIA_AUDIO_SAMPLERATE_BYPASS = 0,          /**< By pass */
  MEDIA_AUDIO_SAMPLERATE_48K = 1,              /**< 48 kHz */
  MEDIA_AUDIO_SAMPLERATE_44_1K = 2,              /**< 44.1 kHz */
  MEDIA_AUDIO_SAMPLERATE_32K = 3,              /**< 32 kHz */
  MEDIA_AUDIO_SAMPLERATE_24K = 4,              /**< 24 kHz */
  MEDIA_AUDIO_SAMPLERATE_16K = 5,              /**< 16 kHz */
  MEDIA_AUDIO_SAMPLERATE_12K = 6,              /**< 12 kHz */
  MEDIA_AUDIO_SAMPLERATE_8K = 7,              /**< 8 kHz */
  MEDIA_AUDIO_SAMPLERATE_22_05K = 8                 /**< 22.05 kHz */
} MEDIA_AUDIO_SAMPLERATE_T;

typedef struct
{
  /* AAC - instead of ADTS header */
  guint32 profile;              // from CP
  guint32 channels;             // from CP
  MEDIA_AUDIO_SAMPLERATE_T frequency;   // from CP

  /* to support raw audio es(without header) */
  gchar *codec_data;            // codec_data : DSI (Decoding Specific Info)    // from CP
  guint32 codec_data_size;      // from CP
} MEDIA_AUDIO_AAC_INFO_T;

typedef struct
{
  /* WMA */
  guint32 wmaVer;
  guint16 wmaFormatTag;
  guint16 channels;
  guint32 samplesPerSec;
  guint32 avgBytesPerSec;
  guint16 blockAlign;
  guint16 bitsPerSample;

  /* to support raw audio es(without header) */
  gchar *codec_data;            // codec_data : DSI (Decoding Specific Info)    // from CP
  guint32 codec_data_size;      // from CP

} MEDIA_AUDIO_WMA_INFO_T;


/**
 * This enumeration describes the bit per sample for PCM.
*
*/
typedef enum
{
  MEDIA_AUDIO_8BIT = 0,          /**< 8 bit per sample */
  MEDIA_AUDIO_16BIT = 1          /**< 16 bit per sample */
} MEDIA_AUDIO_PCM_BIT_PER_SAMPLE_T;

/**
 * This enumeration describes the PCM channel mode.
 *
 */
typedef enum
{
  MEDIA_AUDIO_PCM_MONO = 0,                     /**< PCM mono */
  MEDIA_AUDIO_PCM_DUAL_CHANNEL = 1,             /**< PCM dual channel */
  MEDIA_AUDIO_PCM_STEREO = 2,                 /**< PCM stereo */
  MEDIA_AUDIO_PCM_JOINT_STEREO = 3,             /**< PCM joint stereo */
} MEDIA_AUDIO_PCM_CHANNEL_MODE_T;

/**
 * This structure contains the PCM informations
 *
 */
typedef struct MEDIA_AUDIO_PCM_INFO
{
  MEDIA_AUDIO_PCM_BIT_PER_SAMPLE_T bitsPerSample;             /**< PCM bits per sample */
  MEDIA_AUDIO_SAMPLERATE_T sampleRate;                      /**< PCM sampling rate */
  MEDIA_AUDIO_PCM_CHANNEL_MODE_T channelMode;          /**< PCM channel mode */
} __attribute__ ((packed)) MEDIA_AUDIO_PCM_INFO_T;


/**
 * This enumeration describes extra stream play option type of media. [NetCast40]
 */
typedef enum MEDIA_EXT_CLIPOPT_TYPE
{
  MEDIA_EXT_CLIPOPT_TYPE_NONE = 0,          /**< No opt */
  MEDIA_EXT_CLIPOPT_TYPE_SCREEN_SHARE,      /**< WIDI display INFO */
  MEDIA_EXT_CLIPOPT_TYPE_DLNA,              /**< DLNA INFO */
  MEDIA_EXT_CLIPOPT_TYPE_AD,                /**< AD INFO */
  MEDIA_EXT_CLIPOPT_TYPE_MVPD,              /**< MVPD INFO */
  MEDIA_EXT_CLIPOPT_TYPE_NUM
} MEDIA_EXT_CLIPOPT_TYPE_T;

/**
 * Structure of AD info
 */
typedef struct MEDIA_AD_INFO
{
  gint32 appType;
  gint32 *timeline;
  gint32 numOfTimeline;
} __attribute__ ((packed)) MEDIA_AD_INFO_T;


/**
 * Screen Share Type
 */
typedef enum MEDIA_SCREEN_SHARE_TYPE
{
  MEDIA_SCREEN_SHARE_NONE = 0x00,
  MEDIA_SCREEN_SHARE_WIDI,
  MEDIA_SCREEN_SHARE_WFD,
} MEDIA_SCREEN_SHARE_TYPE_T;



/**
 * Structure of widi info
 */
typedef struct MEDIA_SCREEN_SHARE_INFO
{
  MEDIA_SCREEN_SHARE_TYPE_T shareType;

  guint32 inPort;                    /**< widi display ���� ��� */
  guint32 delayOffset;               /**< delay - ms ���� */

  MEDIA_AUDIO_PCM_BIT_PER_SAMPLE_T bitsPerSample;         /**< PCM bits per sample */
  MEDIA_AUDIO_SAMPLERATE_T sampleRate;                    /**< PCM sampling rate */
  MEDIA_AUDIO_PCM_CHANNEL_MODE_T channelMode;         /**< PCM channel mode */
} __attribute__ ((packed)) MEDIA_SCREEN_SHARE_INFO_T;


/**
 * Structure of DLNA info
 */
typedef struct MEDIA_DLNA_INFO
{
  gchar pProtocolInfo[2048];
  guint64 dContentLength;       //filesize, in byte
  guint32 duration;             //in sec
  guint32 opVal;
  guint32 flagVal;
  guint64 dCleartextSize;       //cleartext size, in byte
} MEDIA_DLNA_INFO_T;

/**
* MVPD : Trick type
*/
typedef enum MEDIA_TRICK_TYPE
{
  MEDIA_TRICK_NONE = 0x00,
  MEDIA_TRICK_CLIENT_SIDE,
  MEDIA_TRICK_SERVER_SIDE,
} MEDIA_TRICK_TYPE_T;

/**
* MVPD : Contents type
*/
typedef enum MEDIA_CONTENT_TYPE
{
  MEDIA_CONTENT_NONE = 0x00,
  MEDIA_CONTENT_LIVE,
  MEDIA_CONTENT_VOD,
} MEDIA_CONTENT_TYPE_T;

/**
* Structure of MVPD info
*/
typedef struct NEDIA_MVPD_INFO
{
  MEDIA_TRICK_TYPE_T trickType;
  MEDIA_CONTENT_TYPE_T contentType;
} NEDIA_MVPD_INFO_T;


/**
 * This enumeration describes encryption type of media.
 */
typedef enum MEDIA_SECURITY_TYPE
{
  MEDIA_SECURITY_NONE = 0,      /**< No encryption */
  MEDIA_SECURITY_AES,           /**< AES */
  MEDIA_SECURITY_WMDRM,         /**< WMDRM */
  MEDIA_SECURITY_VERIMATRIX,    /**< Verimatrix DRM */
  MEDIA_SECURITY_SECUREMEDIA,   /**< Securemedia DRM */
  MEDIA_SECURITY_NUM
} MEDIA_SECURITY_TYPE_T;

/**
 * This enumeration describes preBuffer time sets of media. [NetCast40]
 */
typedef enum MEDIA_CLIPOPT_BUFFER_TIME
{
  MEDIA_CLIPOPT_BUFFER_TIME_DEFAULT = 0,        /**< -1, use for mediapipeline default buffer time. */
  MEDIA_CLIPOPT_BUFFER_TIME_0SEC = 111,             /**<  0, no buffering */
  MEDIA_CLIPOPT_BUFFER_TIME_1SEC = 1,
  MEDIA_CLIPOPT_BUFFER_TIME_2SEC = 2,
  MEDIA_CLIPOPT_BUFFER_TIME_3SEC = 3,
  MEDIA_CLIPOPT_BUFFER_TIME_4SEC = 4,
  MEDIA_CLIPOPT_BUFFER_TIME_5SEC = 5,
  MEDIA_CLIPOPT_BUFFER_TIME_6SEC = 6,
  MEDIA_CLIPOPT_BUFFER_TIME_7SEC = 7,
  MEDIA_CLIPOPT_BUFFER_TIME_8SEC = 8,
  MEDIA_CLIPOPT_BUFFER_TIME_9SEC = 9,
  MEDIA_CLIPOPT_BUFFER_TIME_10SEC = 10,
  MEDIA_CLIPOPT_BUFFER_TIME_11SEC = 11,
  MEDIA_CLIPOPT_BUFFER_TIME_12SEC = 12,
  MEDIA_CLIPOPT_BUFFER_TIME_13SEC = 13,
  MEDIA_CLIPOPT_BUFFER_TIME_14SEC = 14,
  MEDIA_CLIPOPT_BUFFER_TIME_15SEC = 15,
  MEDIA_CLIPOPT_BUFFER_TIME_NUM
} MEDIA_CLIPOPT_BUFFER_TIME_T;

/**
 * Structure of  media play options
 */
typedef struct MEDIA_BUFFERING_CTRL_INFO
{
  guint8 bUseBufferCtrl;        // if it is TRUE, the below time value is meaningful.
  guint8 bufferMinPercent;
  guint8 bufferMaxPercent;
  guint8 bReserved1;            // for memory align.

  MEDIA_CLIPOPT_BUFFER_TIME_T bufferingMinTime;
  MEDIA_CLIPOPT_BUFFER_TIME_T bufferingMaxTime;

} MEDIA_BUFFERING_CTRL_INFO_T;

/**
 * Structure of  media play options
 */
typedef struct MEDIA_CLIPOPT
{
  /* common options */
  MEDIA_TRANSPORT_T mediaTransportType;
  gchar *mediafile;                                                  /**< media URI to play */
  gchar *subtitlefile;                                               /**< media subtitle URI to play */
  MEDIA_SECURITY_TYPE_T securityType;                               /**< Stream encrypt type */
  MEDIA_CLIPOPT_BUFFER_TIME_T preBufferTime;                        /**< Transfer time unit required Pre-Buffering */
  MEDIA_BUFFERING_CTRL_INFO_T bufferingCtrlInfo;                    /**< self buffering monitor */

  /* bufferingOrPlayOnly &  will be used instead */
  MEDIA_PLAYMODE_T bufferingOrPlayOnly;                         /**< buffering&play / bufferingOnly / playOnly */
  guint8 pauseOnEOS;                                             /**< don't stop but pause on EOS */
  guint8 pauseOnBackwardTrickEnd;                                /**< don't start but pause on Backward Trick playback meed position 0. */
  guint8 reservedField1;
  guint8 reservedField2;
  /* DRM & Protocol control option */
  WIDEVINE_OPTION_T widevineOption;                             /**< Widevine option ( asfOption at netcast3.0 )*/

  MEDIA_EXT_CLIPOPT_TYPE_T extraClipOptType;                        /**< MEDIA_SCREEN_SHARE_INFO_T,MEDIA_DLNA_INFO_T,MEDIA_AD_INFO_T,NEDIA_MVPD_INFO_T   */
  MEDIA_AD_INFO_T *pAdInfo;                                           /**< MEDIA_EXT_CLIPOPT_TYPE_AD case  */
  MEDIA_SCREEN_SHARE_INFO_T *pScreenShareInfo;                        /**< MEDIA_EXT_CLIPOPT_TYPE_SCREEN_SHARE case  */
  MEDIA_DLNA_INFO_T *pDlnaInfo;                                       /**< MEDIA_EXT_CLIPOPT_TYPE_DLNA case  */
  NEDIA_MVPD_INFO_T *extOptionMvpd;


} MEDIA_CLIPOPT_T;


/**
 * Structure of static streaming play option
 */
typedef struct MEDIA_STREAMOPT
{

  /* common options */
  MEDIA_TRANSPORT_T mediaTransportType;
  gchar *mediafile;                                                  /**< media URI to play */
  gchar *subtitlefile;                                               /**< media subtitle URI to play */
  MEDIA_SECURITY_TYPE_T securityType;                               /**< Stream encrypt type */
  MEDIA_CLIPOPT_BUFFER_TIME_T preBufferTime;                        /**< Transfer time unit required Pre-Buffering */
  MEDIA_BUFFERING_CTRL_INFO_T bufferingCtrlInfo;                    /**< self buffering monitor */

  /* stream only options */
  MEDIA_FORMAT_T mediaFormatType;
  MEDIA_CODEC_T mediaCodecType;

  guint32 totDataSize;                                       /**< Total streaming data size. ����� Audio������ ���. */
  gint64 ptsToDecode;

  //MEDIA_VIDEO_ADAPTIVE_INFO_T adaptiveInfo;               /**< to support seamless play for adaptive streaming */

  WIDEVINE_OPTION_T widevineOption;                         /**< Widevine option ( asfOption at netcast3.0 )*/
  ES_STREAM_OPT_T ESstreamOption;                              /**< ES stream option */

  /* bufferingOrPlayOnly &  will be used instead - add for html5 youtube */
  MEDIA_PLAYMODE_T bufferingOrPlayOnly;                     /**< buffering&play / bufferingOnly / playOnly */

  guint8 bAdaptiveResolutionStream;                          /**< seperated Resolution  */
  guint8 bRestartStreaming;
  guint8 bSecurityVideoPath;                                 /**< set SVP stream path enable */
  guint8 bIsTTSEngine;                                       /**< set TTS engine type *(for H13 property setting) > */

  MEDIA_EXT_STREAMOPT_TYPE_T extraStreamOptType;                    /**< HOA_AUDIO_AAC_INFO_T,HOA_AUDIO_WMA_INFO_T,HOA_AUDIO_PCM_INFO_T   */
  MEDIA_AUDIO_WMA_INFO_T *pWmaInfo;                                   /**< MEDIA_EXT_STREAMOPT_TYPE_WMA case */
  MEDIA_AUDIO_PCM_INFO_T *pPcmInfo;                                   /**< MEDIA_EXT_STREAMOPT_TYPE_PCM case */
  MEDIA_AUDIO_AAC_INFO_T *pAacInfo;                                   /**< MEDIA_EXT_STREAMOPT_TYPE_AAC case */

} MEDIA_STREAMOPT_T;

typedef enum
{
  MEDIA_OK = 0,
  MEDIA_ERROR = -1,
  MEDIA_NOT_IMPLEMENTED = -2,
  MEDIA_NOT_SUPPORTED = -6,
  MEDIA_BUFFER_FULL = -7,                 /**< ���ۿ� �����Ͱ� ���� ���־� �Լ��� ������� ����  */
  MEDIA_INVALID_PARAMS = -3,             /**< �Լ� ���ڿ� �߸��� ���� ������� */
  MEDIA_NOT_READY = -11,                 /**< API�� resource ���� �غ��� �� */
} MEDIA_STATUS_T;

#endif /* _MEDIA_TYPES_C_H_ */
