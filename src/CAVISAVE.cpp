
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "CAVISAVE.h"
extern "C" {
#include "_tprintd.h"
};

#pragma comment(lib,"avutil-50.lib")
#pragma comment(lib,"avformat-52.lib")
#pragma comment(lib,"avcodec-52.lib")
#pragma comment(lib,"swscale-0.lib")

#pragma comment(lib,"winmm.lib")

typedef struct AVCodecTag {
    int id;
    unsigned int tag;
} AVCodecTag;

const AVCodecTag codec_bmp_tags[] = {
    { CODEC_ID_H264, MKTAG('H', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('h', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('X', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('x', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('a', 'v', 'c', '1') },
    { CODEC_ID_H264, MKTAG('V', 'S', 'S', 'H') },

    { CODEC_ID_H263, MKTAG('H', '2', '6', '3') },
    { CODEC_ID_H263P, MKTAG('H', '2', '6', '3') },
    { CODEC_ID_H263I, MKTAG('I', '2', '6', '3') }, /* intel h263 */
    { CODEC_ID_H261, MKTAG('H', '2', '6', '1') },

    /* added based on MPlayer */
    { CODEC_ID_H263P, MKTAG('U', '2', '6', '3') },
    { CODEC_ID_H263P, MKTAG('v', 'i', 'v', '1') },

    { CODEC_ID_MPEG4, MKTAG('F', 'M', 'P', '4') },
    { CODEC_ID_MPEG4, MKTAG('D', 'I', 'V', 'X') },
    { CODEC_ID_MPEG4, MKTAG('D', 'X', '5', '0') },
    { CODEC_ID_MPEG4, MKTAG('X', 'V', 'I', 'D') },
    { CODEC_ID_MPEG4, MKTAG('M', 'P', '4', 'S') },
    { CODEC_ID_MPEG4, MKTAG('M', '4', 'S', '2') },
    { CODEC_ID_MPEG4, MKTAG(0x04, 0, 0, 0) }, /* some broken avi use this */

    /* added based on MPlayer */
    { CODEC_ID_MPEG4, MKTAG('D', 'I', 'V', '1') },
    { CODEC_ID_MPEG4, MKTAG('B', 'L', 'Z', '0') },
    { CODEC_ID_MPEG4, MKTAG('m', 'p', '4', 'v') },
    { CODEC_ID_MPEG4, MKTAG('U', 'M', 'P', '4') },
    { CODEC_ID_MPEG4, MKTAG('W', 'V', '1', 'F') },
    { CODEC_ID_MPEG4, MKTAG('S', 'E', 'D', 'G') },

    { CODEC_ID_MPEG4, MKTAG('R', 'M', 'P', '4') },

    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '3') }, /* default signature when using MSMPEG4 */
    { CODEC_ID_MSMPEG4V3, MKTAG('M', 'P', '4', '3') },

    /* added based on MPlayer */
    { CODEC_ID_MSMPEG4V3, MKTAG('M', 'P', 'G', '3') },
    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '5') },
    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '6') },
    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '4') },
    { CODEC_ID_MSMPEG4V3, MKTAG('A', 'P', '4', '1') },
    { CODEC_ID_MSMPEG4V3, MKTAG('C', 'O', 'L', '1') },
    { CODEC_ID_MSMPEG4V3, MKTAG('C', 'O', 'L', '0') },

    { CODEC_ID_MSMPEG4V2, MKTAG('M', 'P', '4', '2') },

    /* added based on MPlayer */
    { CODEC_ID_MSMPEG4V2, MKTAG('D', 'I', 'V', '2') },

    { CODEC_ID_MSMPEG4V1, MKTAG('M', 'P', 'G', '4') },

    { CODEC_ID_WMV1, MKTAG('W', 'M', 'V', '1') },

    /* added based on MPlayer */
    { CODEC_ID_WMV2, MKTAG('W', 'M', 'V', '2') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 's', 'd') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'h', 'd') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 's', 'l') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', '2', '5') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('m', 'p', 'g', '1') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('m', 'p', 'g', '2') },
    { CODEC_ID_MPEG2VIDEO, MKTAG('m', 'p', 'g', '2') },
    { CODEC_ID_MPEG2VIDEO, MKTAG('M', 'P', 'E', 'G') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('P', 'I', 'M', '1') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('V', 'C', 'R', '2') },
    { CODEC_ID_MPEG1VIDEO, 0x10000001 },
    { CODEC_ID_MPEG2VIDEO, 0x10000002 },
    { CODEC_ID_MPEG2VIDEO, MKTAG('D', 'V', 'R', ' ') },
    { CODEC_ID_MPEG2VIDEO, MKTAG('M', 'M', 'E', 'S') },
    { CODEC_ID_MJPEG, MKTAG('M', 'J', 'P', 'G') },
    { CODEC_ID_MJPEG, MKTAG('L', 'J', 'P', 'G') },
    { CODEC_ID_LJPEG, MKTAG('L', 'J', 'P', 'G') },
    { CODEC_ID_MJPEG, MKTAG('J', 'P', 'G', 'L') }, /* Pegasus lossless JPEG */
    { CODEC_ID_MJPEG, MKTAG('M', 'J', 'L', 'S') }, /* JPEG-LS custom FOURCC for avi - decoder */
    { CODEC_ID_MJPEG, MKTAG('j', 'p', 'e', 'g') },
    { CODEC_ID_MJPEG, MKTAG('I', 'J', 'P', 'G') },
    { CODEC_ID_MJPEG, MKTAG('A', 'V', 'R', 'n') },
    { CODEC_ID_HUFFYUV, MKTAG('H', 'F', 'Y', 'U') },
    { CODEC_ID_FFVHUFF, MKTAG('F', 'F', 'V', 'H') },
    { CODEC_ID_CYUV, MKTAG('C', 'Y', 'U', 'V') },
    { CODEC_ID_RAWVIDEO, 0 },
    { CODEC_ID_RAWVIDEO, MKTAG('I', '4', '2', '0') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', 'U', 'Y', '2') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', '4', '2', '2') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', 'V', '1', '2') },
    { CODEC_ID_RAWVIDEO, MKTAG('U', 'Y', 'V', 'Y') },
    { CODEC_ID_RAWVIDEO, MKTAG('I', 'Y', 'U', 'V') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', '8', '0', '0') },
    { CODEC_ID_RAWVIDEO, MKTAG('H', 'D', 'Y', 'C') },
    { CODEC_ID_INDEO3, MKTAG('I', 'V', '3', '1') },
    { CODEC_ID_INDEO3, MKTAG('I', 'V', '3', '2') },
    { CODEC_ID_VP3, MKTAG('V', 'P', '3', '1') },
    { CODEC_ID_VP3, MKTAG('V', 'P', '3', '0') },
    { CODEC_ID_ASV1, MKTAG('A', 'S', 'V', '1') },
    { CODEC_ID_ASV2, MKTAG('A', 'S', 'V', '2') },
    { CODEC_ID_VCR1, MKTAG('V', 'C', 'R', '1') },
    { CODEC_ID_FFV1, MKTAG('F', 'F', 'V', '1') },
    { CODEC_ID_XAN_WC4, MKTAG('X', 'x', 'a', 'n') },
    { CODEC_ID_MSRLE, MKTAG('m', 'r', 'l', 'e') },
    { CODEC_ID_MSRLE, MKTAG(0x1, 0x0, 0x0, 0x0) },
    { CODEC_ID_MSVIDEO1, MKTAG('M', 'S', 'V', 'C') },
    { CODEC_ID_MSVIDEO1, MKTAG('m', 's', 'v', 'c') },
    { CODEC_ID_MSVIDEO1, MKTAG('C', 'R', 'A', 'M') },
    { CODEC_ID_MSVIDEO1, MKTAG('c', 'r', 'a', 'm') },
    { CODEC_ID_MSVIDEO1, MKTAG('W', 'H', 'A', 'M') },
    { CODEC_ID_MSVIDEO1, MKTAG('w', 'h', 'a', 'm') },
    { CODEC_ID_CINEPAK, MKTAG('c', 'v', 'i', 'd') },
    { CODEC_ID_TRUEMOTION1, MKTAG('D', 'U', 'C', 'K') },
    { CODEC_ID_MSZH, MKTAG('M', 'S', 'Z', 'H') },
    { CODEC_ID_ZLIB, MKTAG('Z', 'L', 'I', 'B') },
    { CODEC_ID_SNOW, MKTAG('S', 'N', 'O', 'W') },
    { CODEC_ID_4XM, MKTAG('4', 'X', 'M', 'V') },
    { CODEC_ID_FLV1, MKTAG('F', 'L', 'V', '1') },
    { CODEC_ID_SVQ1, MKTAG('s', 'v', 'q', '1') },
    { CODEC_ID_TSCC, MKTAG('t', 's', 'c', 'c') },
    { CODEC_ID_ULTI, MKTAG('U', 'L', 'T', 'I') },
    { CODEC_ID_VIXL, MKTAG('V', 'I', 'X', 'L') },
    { CODEC_ID_QPEG, MKTAG('Q', 'P', 'E', 'G') },
    { CODEC_ID_QPEG, MKTAG('Q', '1', '.', '0') },
    { CODEC_ID_QPEG, MKTAG('Q', '1', '.', '1') },
    { CODEC_ID_WMV3, MKTAG('W', 'M', 'V', '3') },
    { CODEC_ID_LOCO, MKTAG('L', 'O', 'C', 'O') },
    { CODEC_ID_THEORA, MKTAG('t', 'h', 'e', 'o') },
#if LIBAVCODEC_VERSION_INT>0x000409
    { CODEC_ID_WNV1, MKTAG('W', 'N', 'V', '1') },
    { CODEC_ID_AASC, MKTAG('A', 'A', 'S', 'C') },
    { CODEC_ID_INDEO2, MKTAG('R', 'T', '2', '1') },
    { CODEC_ID_FRAPS, MKTAG('F', 'P', 'S', '1') },
    { CODEC_ID_TRUEMOTION2, MKTAG('T', 'M', '2', '0') },
#endif
#if LIBAVCODEC_VERSION_INT>((50<<16)+(1<<8)+0)
    { CODEC_ID_FLASHSV, MKTAG('F', 'S', 'V', '1') },
    { CODEC_ID_JPEGLS,MKTAG('M', 'J', 'L', 'S') }, /* JPEG-LS custom FOURCC for avi - encoder */
    { CODEC_ID_VC1, MKTAG('W', 'V', 'C', '1') },
    { CODEC_ID_VC1, MKTAG('W', 'M', 'V', 'A') },
    { CODEC_ID_CSCD, MKTAG('C', 'S', 'C', 'D') },
    { CODEC_ID_ZMBV, MKTAG('Z', 'M', 'B', 'V') },
    { CODEC_ID_KMVC, MKTAG('K', 'M', 'V', 'C') },
#endif
#if LIBAVCODEC_VERSION_INT>((51<<16)+(11<<8)+0)
    { CODEC_ID_VP5, MKTAG('V', 'P', '5', '0') },
    { CODEC_ID_VP6, MKTAG('V', 'P', '6', '0') },
    { CODEC_ID_VP6, MKTAG('V', 'P', '6', '1') },
    { CODEC_ID_VP6, MKTAG('V', 'P', '6', '2') },
    { CODEC_ID_VP6F, MKTAG('V', 'P', '6', 'F') },
    { CODEC_ID_JPEG2000, MKTAG('M', 'J', '2', 'C') },
    { CODEC_ID_VMNC, MKTAG('V', 'M', 'n', 'c') },
#endif
#if LIBAVCODEC_VERSION_INT>=((51<<16)+(49<<8)+0)
// this tag seems not to exist in older versions of FFMPEG
    { CODEC_ID_TARGA, MKTAG('t', 'g', 'a', ' ') },
#endif
    { CODEC_ID_NONE, 0 },
};

// ビデオストリームを追加する
AVStream *ffmpeg_addVStream( AVFormatContext *oc,
		                                     CodecID codec_id ,
											 int w, int h, int bitrate,
											 double fps, int pixel_format)
{
	AVCodecContext *c;
	AVStream *st;
	int frame_rate, frame_rate_base;
	AVCodec *codec;


	st = av_new_stream(oc, oc->nb_streams);
	if (!st) {
		return NULL;
	}

#if LIBAVFORMAT_BUILD > 4628
	c = st->codec;
#else
	c = &(st->codec);
#endif

#if LIBAVFORMAT_BUILD > 4621
	c->codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, CODEC_TYPE_VIDEO);
#else
	c->codec_id = oc->oformat->video_codec;
#endif

	if(codec_id != CODEC_ID_NONE){
		c->codec_id = codec_id;
	}

    //if(codec_tag) c->codec_tag=codec_tag;
	codec = avcodec_find_encoder(c->codec_id);

	c->codec_type = CODEC_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = bitrate;

	/* resolution must be a multiple of two */
	c->width = w;
	c->height = h;

	/* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
	frame_rate=(int)fps;
	frame_rate_base=1;
	while (fabs((double)frame_rate/frame_rate_base) - fps > 0.001){
		frame_rate_base*=10;
		frame_rate=(int)(fps*frame_rate_base);
	}
#if LIBAVFORMAT_BUILD > 4752
    c->time_base.den = frame_rate;
    c->time_base.num = frame_rate_base;
	/* adjust time base for supported framerates */
	if(codec && codec->supported_framerates){
		const AVRational *p= codec->supported_framerates;
        AVRational req = {frame_rate, frame_rate_base};
		const AVRational *best=NULL;
		AVRational best_error= {INT_MAX, 1};
		for(; p->den!=0; p++){
			AVRational error= av_sub_q(req, *p);
			if(error.num <0) error.num *= -1;
			if(av_cmp_q(error, best_error) < 0){
				best_error= error;
				best= p;
			}
		}
		c->time_base.den= best->num;
		c->time_base.num= best->den;
	}
#else
	c->frame_rate = frame_rate;
	c->frame_rate_base = frame_rate_base;
#endif

	c->gop_size = 12; /* emit one intra frame every twelve frames at most */
	c->pix_fmt = (PixelFormat) pixel_format;

	if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO || c->codec_id == CODEC_ID_MSMPEG4V3){
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
		/* avoid FFMPEG warning 'clipping 1 dct coefficients...' */
        c->mb_decision=2;
    }
#if LIBAVCODEC_VERSION_INT>0x000409
    // some formats want stream headers to be seperate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
    {
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
#endif

    return st;
}

// オーディオストリームを追加する
AVStream* ffmpeg_addAStream(AVFormatContext *oc, int codec_id, int sample_rate , int channels , int bit_rate ){
	AVCodecContext *c;
	AVStream *st;

	st = av_new_stream(oc, oc->nb_streams);
	if (!st) {
		fprintf(stderr, "Could not alloc stream\n");
		return NULL;
	}
	c = st->codec;
	c->codec_id = (CodecID)codec_id;
	c->codec_type = CODEC_TYPE_AUDIO;
	c->bit_rate = bit_rate;

	/*put sample parameters*/
   if(c->codec_id == CODEC_ID_AMR_NB) {
	  c->sample_rate = 8000;
	  c->channels = 1;
   }else {
	  c->sample_rate = sample_rate;
	  c->channels = channels;
   }
	return st;
}

/**
 * the following function is a modified version of code
 * found in ffmpeg-0.4.9-pre1/output_example.c
 */
static AVFrame * icv_alloc_picture_FFMPEG(int pix_fmt, int width, int height, bool alloc)
{
	AVFrame * picture;
	uint8_t * picture_buf;
	int size;

	picture = avcodec_alloc_frame();
	if (!picture)
		return NULL;
	size = avpicture_get_size( (PixelFormat) pix_fmt, width, height);
	if(alloc){
		picture_buf = (uint8_t *) new unsigned char[size];
		if (!picture_buf)
		{
			av_free(picture);
			return NULL;
		}
		avpicture_fill((AVPicture *)picture, picture_buf,
				(PixelFormat) pix_fmt, width, height);
	} else {
	}
	return picture;
}

int icv_av_write_frame_FFMPEG( AVFormatContext * oc, AVStream * video_st, uint8_t * outbuf, uint32_t outbuf_size, AVFrame * picture ){

#if LIBAVFORMAT_BUILD > 4628
	AVCodecContext * c = video_st->codec;
#else
	AVCodecContext * c = &(video_st->codec);
#endif
	int out_size;
	int ret;

    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= video_st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        ret = av_write_frame(oc, &pkt);
    } else {
        /* encode the image */
        out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);

#if LIBAVFORMAT_BUILD > 4752
            pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, video_st->time_base);
#else
			pkt.pts = c->coded_frame->pts;
#endif
            if(c->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= video_st->index;
            pkt.data= outbuf;
            pkt.size= out_size;

            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
		// error
		return -1;
	}

	return 0;
}


int ffmpeginit( _ffmpeg_struct* pffm , char* pFilename , CodecID vcodec_id , long width , long height , double fps , int vbitrate, CodecID acodec_id , int sample_rate , int channels , int audio_bit_rate ){
	if( !(pffm && pFilename) ) return -1;
//	unsigned int codec_tag = MKTAG('X', 'V', 'I', 'D');
//	unsigned int codec_tag = MKTAG('D', 'X', '5', '0');
	
	int err, codec_pix_fmt;
	AVCodec			*pACodec;

	pffm->m_pFmt = guess_format(NULL, pFilename, NULL);
	if( pffm->m_pFmt == NULL )
		return -1;
	printf( "Name       :%s\n" , pffm->m_pFmt->name );
	printf( "LongName   :%s\n" , pffm->m_pFmt->long_name );
	printf( "MimeType   :%s\n" , pffm->m_pFmt->mime_type );
	printf( "extensions :%s\n" , pffm->m_pFmt->extensions );
	
	int input_pix_fmt = PIX_FMT_BGR24;
	
	pffm->m_pOutFmtContext = av_alloc_format_context();
	pffm->m_pOutFmtContext->oformat = pffm->m_pFmt;
#if _MSC_VER >= 1400
	sprintf_s(pffm->m_pOutFmtContext->filename, sizeof(pffm->m_pOutFmtContext->filename), "%s", pFilename);
#else
	sprintf( pffm->m_pOutFmtContext->filename, "%s", pFilename);
#endif
	
	pffm->m_pOutFmtContext->max_delay = (int)(0.7*AV_TIME_BASE);
	
	pffm->m_Audio.m_pDCodecCtx = avcodec_alloc_context();
	if( pffm->m_Audio.m_pDCodecCtx == NULL ){
		return -1;
	}
	
	pACodec = avcodec_find_encoder( CODEC_ID_PCM_S16LE );
	if(pACodec == NULL)
		return -1; // Codec not found
	
	// コーディックを開く
	if(avcodec_open(pffm->m_Audio.m_pDCodecCtx, pACodec)<0)
		return -1; // Could not open codec
	
	pffm->m_Audio.m_pDCodecCtx->sample_rate	= sample_rate;
	pffm->m_Audio.m_pDCodecCtx->channels	= channels;
	pffm->m_Audio.m_pDCodecCtx->sample_fmt	= SAMPLE_FMT_S16;
	
	// コーディックを取得する
	if( acodec_id != CODEC_ID_NONE )
		pACodec = avcodec_find_encoder( (CodecID)acodec_id );
	else
		pACodec = avcodec_find_encoder( pffm->m_pFmt->audio_codec );
	if(pACodec == NULL)
		return -1; // Codec not found
	
	pffm->m_pAStream = ffmpeg_addAStream( pffm->m_pOutFmtContext , pffm->m_pFmt->audio_codec , pffm->m_Audio.m_pDCodecCtx->sample_rate  , pffm->m_Audio.m_pDCodecCtx->channels , audio_bit_rate );
	pffm->m_Audio.m_pECodecCtx = pffm->m_pAStream->codec;
	// コーディックを開く
	if(avcodec_open(pffm->m_Audio.m_pECodecCtx, pACodec)<0)
		return -1; // Could not open codec
	
	
	/* Lookup codec_id for given fourcc */
	/*
#if LIBAVCODEC_VERSION_INT<((51<<16)+(49<<8)+0)
        if( (vcodec_id = codec_get_bmp_id( codec_tag )) == CODEC_ID_NONE ){
			// error
			pffm = pffm;
			return -1;
		}
#else
	{
	const struct AVCodecTag * tags[] = { codec_bmp_tags, NULL};
        if( (vcodec_id = av_codec_get_id(tags, codec_tag )) == CODEC_ID_NONE ){
			// error
			pffm = pffm;
			return -1;
		}
	}
#endif*/
	if( vcodec_id == CODEC_ID_NONE )
		vcodec_id = pffm->m_pFmt->video_codec;
	
    switch (vcodec_id) {
#if LIBAVCODEC_VERSION_INT>((50<<16)+(1<<8)+0)
    case CODEC_ID_JPEGLS:
        // BGR24 or GRAY8 depending on is_color...
        codec_pix_fmt = input_pix_fmt;
        break;
#endif
    case CODEC_ID_HUFFYUV:
        codec_pix_fmt = PIX_FMT_YUV422P;
        break;
    case CODEC_ID_MJPEG:
    case CODEC_ID_LJPEG:
      codec_pix_fmt = PIX_FMT_YUVJ420P;
      vbitrate *= 2;
      break;
    case CODEC_ID_RAWVIDEO:
    default:
        // good for lossy formats, MPEG, etc.
        codec_pix_fmt = PIX_FMT_YUV420P;
        break;
    }
	
	pffm->m_pVStream = ffmpeg_addVStream( pffm->m_pOutFmtContext , vcodec_id , width , height , vbitrate , fps , codec_pix_fmt );
	
	if (av_set_parameters(pffm->m_pOutFmtContext, NULL) < 0) {
		// error
		return -1;
    }
	
	dump_format(pffm->m_pOutFmtContext, 0, pFilename, 1);
	
    if (!pffm->m_pVStream){
		// error
		return -1;
	}
	
    AVCodec *vcodec;
    AVCodecContext *vc;

#if LIBAVFORMAT_BUILD > 4628
    vc = (pffm->m_pVStream->codec);
#else
    vc = &(pffm->m_pVStream->codec);
#endif

//    vc->codec_tag = codec_tag;
    /* find the video encoder */
    vcodec = avcodec_find_encoder(vc->codec_id);
    if (!vcodec) {
		// error
		return -1;
    }

    /* open the codec */
    if ( (err=avcodec_open(vc, vcodec)) < 0) {
		// error
		return -1;
	}

    pffm->m_Video.m_pOutBuf = NULL;

    if (!(pffm->m_pOutFmtContext->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
		/* assume we will never get codec output with more than 4 bytes per pixel... */
		pffm->m_Video.m_OutBufSize = width*height*4;
        pffm->m_Video.m_pOutBuf = (uint8_t *) av_malloc(pffm->m_Video.m_OutBufSize);
    }

	bool need_color_convert;
	need_color_convert = (vc->pix_fmt != input_pix_fmt);

    /* allocate the encoded raw picture */
    pffm->m_Video.m_pPicture = icv_alloc_picture_FFMPEG(vc->pix_fmt, vc->width, vc->height, need_color_convert);
    if (!pffm->m_Video.m_pPicture) {
		// error
		return -1;
    }

    /* if the output format is not our input format, then a temporary
       picture of the input format is needed too. It is then converted
	   to the required output format */
	pffm->m_Video.m_pInputPicture = NULL;
    if ( need_color_convert ) {
        pffm->m_Video.m_pInputPicture = icv_alloc_picture_FFMPEG(input_pix_fmt, vc->width, vc->height, false);
        if (!pffm->m_Video.m_pInputPicture) {
			// error
			return -1;
        }
    }

	/* open the output file, if needed */
    if (!(pffm->m_pFmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&pffm->m_pOutFmtContext->pb, pFilename, URL_WRONLY) < 0) {
			// error
			return -1;
        }
    }

    /* write the stream header, if any */
    av_write_header( pffm->m_pOutFmtContext );
	
	av_init_packet(&pffm->m_Audio.pkt);
	
	
	#define MAX_AUDIO_PACKET_SIZE (512 * 1024)
	pffm->m_Audio.m_fifo = av_fifo_alloc(2 * MAX_AUDIO_PACKET_SIZE);
	
	// バッファを確保
	pffm->m_Audio.m_tmpbuf.data_buf = (uint8_t*)av_malloc(2*MAX_AUDIO_PACKET_SIZE);
	pffm->m_Audio.m_tmpbuf.audio_out = (uint8_t*)av_malloc(4*MAX_AUDIO_PACKET_SIZE);
	pffm->m_Audio.m_tmpbuf.buffer_resample = (uint8_t*)av_malloc(2*MAX_AUDIO_PACKET_SIZE);

	

	// サンプリングレートまたはチャンネル数が違う場合は、リサンプルする
	if(pffm->m_Audio.m_pDCodecCtx->sample_rate != pffm->m_Audio.m_pECodecCtx->sample_rate || pffm->m_Audio.m_pDCodecCtx->channels != pffm->m_Audio.m_pECodecCtx->channels)
		pffm->m_Audio.m_rs = audio_resample_init(pffm->m_Audio.m_pECodecCtx->channels,pffm->m_Audio.m_pDCodecCtx->channels, pffm->m_Audio.m_pECodecCtx->sample_rate, pffm->m_Audio.m_pDCodecCtx->sample_rate);

	return 0;
}

void ffmpegquit( _ffmpeg_struct* pffm ){
	if( !(pffm) ) return;
	
	av_write_trailer(pffm->m_pOutFmtContext);
	
	// ここから解放処理
	if( pffm->m_Audio.m_rs ){
		audio_resample_close( pffm->m_Audio.m_rs );
		pffm->m_Audio.m_rs = NULL;
	}
	if( pffm->m_Audio.m_tmpbuf.buffer_resample ){
		av_free(pffm->m_Audio.m_tmpbuf.buffer_resample);
		pffm->m_Audio.m_tmpbuf.buffer_resample = NULL;
	}
	if( pffm->m_Audio.m_tmpbuf.audio_out ){
		av_free(pffm->m_Audio.m_tmpbuf.audio_out);
		pffm->m_Audio.m_tmpbuf.audio_out = NULL;
	}
	if( pffm->m_Audio.m_tmpbuf.data_buf ){
		av_free(pffm->m_Audio.m_tmpbuf.data_buf);
		pffm->m_Audio.m_tmpbuf.data_buf = NULL;
	}
	
	if( pffm->m_Audio.m_fifo ){
		av_fifo_free(pffm->m_Audio.m_fifo);
		pffm->m_Audio.m_fifo = NULL;
	}
	
	if( pffm->m_Audio.m_pDCodecCtx ){
		avcodec_close( pffm->m_Audio.m_pDCodecCtx );
		pffm->m_Audio.m_pDCodecCtx = NULL;
	}
	if( pffm->m_Audio.m_pECodecCtx ){
		avcodec_close( pffm->m_Audio.m_pECodecCtx );
		pffm->m_Audio.m_pECodecCtx = NULL;
	}
	
	
	if (pffm->m_Video.m_pInputPicture) {
#if LIBAVFORMAT_BUILD > 4628
		if( pffm->m_pVStream->codec->pix_fmt != PIX_FMT_BGR24){
#else
		if( pffm->m_pVStream->codec.pix_fmt != PIX_FMT_BGR24){
#endif
			delete [] pffm->m_Video.m_pPicture->data[0];
		}
		av_free(pffm->m_Video.m_pPicture);
		pffm->m_Video.m_pPicture = NULL;
	}
	
	if (pffm->m_Video.m_pInputPicture) {
		av_free(pffm->m_Video.m_pInputPicture);
		pffm->m_Video.m_pInputPicture = NULL;
	}
	//
	if( pffm->m_Video.m_pOutBuf ){
		av_free(pffm->m_Video.m_pOutBuf);
		pffm->m_Video.m_pOutBuf = NULL;
		pffm->m_Video.m_OutBufSize = 0;
	}

#if LIBAVFORMAT_BUILD > 4628
	avcodec_close(pffm->m_pVStream->codec);
	avcodec_close(pffm->m_pAStream->codec);
#else
	avcodec_close(&(pffm->m_pVStream->codec));
	avcodec_close(&(pffm->m_pAStream->codec));
#endif

	av_free(pffm->m_Video.m_pOutBuf );

	if( pffm->m_Video.m_img_convert_ctx ){
		sws_freeContext( pffm->m_Video.m_img_convert_ctx );
		pffm->m_Video.m_img_convert_ctx = NULL;
	}
	
	if( pffm->m_pOutFmtContext ){
		/* free the streams */
		for(unsigned int i = 0; i < pffm->m_pOutFmtContext->nb_streams; i++) {
			av_freep(&pffm->m_pOutFmtContext->streams[i]->codec);
			av_freep(&pffm->m_pOutFmtContext->streams[i]);
		}
		pffm->m_pAStream = NULL;
		pffm->m_pVStream = NULL;
		
		if (!(pffm->m_pFmt->flags & AVFMT_NOFILE)) {
			/* close the output file */
#if LIBAVCODEC_VERSION_INT >= ((51<<16)+(49<<8)+0)
			url_fclose(pffm->m_pOutFmtContext->pb);
#else
			url_fclose(&pffm->m_pOutFmtContext->pb);
#endif
			pffm->m_pOutFmtContext->pb = NULL;

		}

		av_free( pffm->m_pOutFmtContext );
		pffm->m_pOutFmtContext = NULL;
	}
	
}

int ffmpeg_addVFrame( _ffmpeg_struct* pffm , unsigned char *pBuf , long width , long height ){
	if( !(pffm) ) return -1;
	
	//
#if LIBAVFORMAT_BUILD > 4628
	AVCodecContext *c = pffm->m_pVStream->codec;
#else
	AVCodecContext *c = &(pffm->m_pVStream->codec);
#endif
	
	c = c;
	if ( c->pix_fmt != PIX_FMT_BGR24 ) {
		// 画像形式を変換する必要がある
		avpicture_fill((AVPicture *)pffm->m_Video.m_pInputPicture , (uint8_t *) pBuf ,
			(PixelFormat)PIX_FMT_BGR24, width, height);
		pffm->m_Video.m_img_convert_ctx = sws_getContext(width,
		             height,
		             PIX_FMT_BGR24,
		             c->width,
		             c->height,
		             c->pix_fmt,
		             SWS_BICUBIC,
		             NULL, NULL, NULL);

#if _MSC_VER >= 1400
		    if ( sws_scale(pffm->m_Video.m_img_convert_ctx, pffm->m_Video.m_pInputPicture->data,
		             pffm->m_Video.m_pInputPicture->linesize, 0,
		             height,
		             pffm->m_Video.m_pPicture->data, pffm->m_Video.m_pPicture->linesize) < 0 )
#else
		    if ( sws_scale(pffm->m_Video.m_img_convert_ctx, (const uint8_t**)pffm->m_Video.m_pInputPicture->data,
		             pffm->m_Video.m_pInputPicture->linesize, 0,
		             height,
		             pffm->m_Video.m_pPicture->data, pffm->m_Video.m_pPicture->linesize) < 0 )
#endif
		    {
				// error
				_tprintd( _T("Error sws_scale\n") );
		    }
		sws_freeContext(pffm->m_Video.m_img_convert_ctx);
		pffm->m_Video.m_img_convert_ctx = NULL;

	}else{
		avpicture_fill((AVPicture *)pffm->m_Video.m_pPicture, (uint8_t *) pBuf,
				(PixelFormat)PIX_FMT_BGR24, width, height);
	}
	
	bool ret = false;
	ret = icv_av_write_frame_FFMPEG( pffm->m_pOutFmtContext, pffm->m_pVStream, pffm->m_Video.m_pOutBuf, pffm->m_Video.m_OutBufSize, pffm->m_Video.m_pPicture) >= 0;
	
	return 0;
	//
}

int ffmpeg_addAFrame( _ffmpeg_struct* pffm , unsigned char *pBuf , size_t bufsize ){
		static unsigned int samples_size= 0;
		// Is this a packet from the audio stream?
//			samples = (short *) av_fast_realloc(samples, &samples_size, FFMAX(blocksize, AVCODEC_MAX_AUDIO_FRAME_SIZE)); //192000
//			int audiobufsize = AVCODEC_MAX_AUDIO_FRAME_SIZE*2;
//			avcodec_decode_audio2(pCodecCtx, samples, &audiobufsize, pBuf, blocksize);
//			memset( samples , 0 , samples_size );

			// fifoに保存する？
/*			if(pWrite->m_pDCodecCtx->sample_rate != pWrite->m_pECodecCtx->sample_rate || pWrite->m_pDCodecCtx->channels != pWrite->m_pECodecCtx->channels) {
				// リサンプリングしてfifoに保存する？
				int size_out = audio_resample(pWrite->m_rs, (short *)pWrite->m_tmpbuf.buffer_resample, samples, blocksize/ (pWrite->m_pDCodecCtx->channels * 2) );
				av_fifo_generic_write(pWrite->m_fifo, (uint8_t *)pWrite->m_tmpbuf.buffer_resample, size_out * pWrite->m_pECodecCtx->channels * 2, NULL );
			}else*/
				// fifoに保存する？
				av_fifo_generic_write(pffm->m_Audio.m_fifo, (uint8_t *)pBuf, bufsize, NULL );

			// Did we get a audio frame?
			if(bufsize){
				int frame_bytes = pffm->m_Audio.m_pECodecCtx->frame_size * 2 * pffm->m_Audio.m_pECodecCtx->channels;

				while( av_fifo_size(pffm->m_Audio.m_fifo) >= frame_bytes ) {

					int ret = av_fifo_generic_read( pffm->m_Audio.m_fifo, pffm->m_Audio.m_tmpbuf.data_buf, frame_bytes, NULL );	  
					pffm->m_Audio.pkt.size= avcodec_encode_audio(pffm->m_Audio.m_pECodecCtx, pffm->m_Audio.m_tmpbuf.audio_out, frame_bytes , (short *)pffm->m_Audio.m_tmpbuf.data_buf);

					//pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, audio_st->time_base);
					pffm->m_Audio.pkt.stream_index	= pffm->m_pAStream->index;
					pffm->m_Audio.pkt.data			= pffm->m_Audio.m_tmpbuf.audio_out;
					pffm->m_Audio.pkt.flags			|= PKT_FLAG_KEY;
					
					// ファイルに圧縮されたフレームを書く
					if (av_interleaved_write_frame(pffm->m_pOutFmtContext, &pffm->m_Audio.pkt) != 0) {
						fprintf(stderr, "Error while writing audio frame\n");
						return -1;
					}
				} // while( av_fifo_size(fifo) >= frame_bytes ) {
			} // if(audiobufsize){

		// Free the packet that was allocated by av_read_frame
		av_free_packet(&pffm->m_Audio.pkt);
		return 0;
}
