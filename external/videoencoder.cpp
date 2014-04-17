/*
 * This file is part of DLM.
 *
 * DLM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DLM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DLM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "videoencoder.h"

#include <QThread>
#include <QTime>
#include <QVector>
#include <QByteArray>
#include <QSize>
#include <QImage>
#include <QDebug>

static void convertRGB888toYUV420P(
        uint8_t const* src, uint8_t** dst, int width, int height) {
#define sR ((unsigned)(s[0]))
#define sG ((unsigned)(s[1]))
#define sB ((unsigned)(s[2]))
  int y = 0, uv = 0;
  uint8_t const* s = src;
  for(int h = 0; h < height; ++h) {
    for(int w = 0; w < width; ++w) {
      dst[0][y] = ((66 * sR + 129 * sG +  25 * sB + 128) >> 8) + 16;
      if(h%2 == 0 && w%2 == 0) {
        dst[1][uv] = ((-38 * sR - 74 * sG + 112 * sB + 128) >> 8) + 128;
        dst[2][uv] = ((112 * sR - 94 * sG - 18 * sB + 128) >> 8) + 128;
        ++uv;
      }
      ++y;
      s += 3;
    }
  }
#undef sR
#undef sG
#undef sB
}

VideoEncoder::VideoEncoder(const QSize &imageSize, int fps, int bitrate, int preset_index)
{
    x264_param_default_preset(&param, x264_preset_names[preset_index], "zerolatency");

    //param.i_log_level = X264_LOG_ERROR;
    param.i_threads = QThread::idealThreadCount();
    param.i_width = imageSize.width();
    param.i_height = imageSize.height();
    param.i_fps_num = fps;
    param.i_fps_den = 1;
    // Intra refres:
    param.i_keyint_max = fps * 4;
    //param.b_intra_refresh = 1;
    // Rate control
    param.rc.i_rc_method = X264_RC_ABR;
    //param.rc.f_rf_constant = 25;
    //param.rc.f_rf_constant_max = 35;
    param.rc.i_bitrate = bitrate;
    //For streaming
    param.b_repeat_headers = 0;
    param.b_annexb = 0;
    //param.i_log_level = X264_LOG_DEBUG;
    x264_param_apply_profile(&param, "main");

    encoder = x264_encoder_open(&param); //param is copied into encoder
    x264_picture_alloc(&pic_in, X264_CSP_I420, param.i_width, param.i_height);
}

VideoEncoder::~VideoEncoder() throw()
{
    x264_encoder_close(encoder);
    x264_picture_clean(&pic_in);
}

QByteArray VideoEncoder::flvSequenceHeader()
{
    x264_encoder_headers( encoder, &nals, &i_nals );
    QVector<QByteArray> sps, pps;
    for(int i = 0 ; i < i_nals ; ++i )
    {
        switch( nals[i].i_type )
        { // +4, -4, removes the 4 byte size at teh begining for the NALU
        case 7: sps.append( QByteArray((const char*)nals[i].p_payload + 4, nals[i].i_payload - 4) ); break;
        case 8: pps.append( QByteArray((const char*)nals[i].p_payload + 4, nals[i].i_payload - 4) ); break;
        }
    }

    // TODO make sure we have sps and pps
    QByteArray sequenceHeader;
    sequenceHeader.append( 0x01 );  // version
    sequenceHeader.append( sps[0][1] ); // profile
    sequenceHeader.append( sps[0][2] ); // compatibility
    sequenceHeader.append( sps[0][3] ); // level
    sequenceHeader.append(0xFC | 3); // reserved (6 bits), NULA length size - 1 (2 bits)
    sequenceHeader.append(0xE0 | sps.size() ); // reserved (3 bits), num of SPS (5 bits)
    for(int i = 0 ; i < sps.size() ; ++i )
    {
        quint16 size = sps[i].size();
        sequenceHeader.append( 0xFF & ( size >> 8 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( 0xFF & ( size >> 0 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( sps[i] );
    }

    sequenceHeader.append( pps.size() );
    for(int i = 0 ; i < pps.size() ; ++i )
    {
        quint16 size = pps[i].size();
        sequenceHeader.append( 0xFF & ( size >> 8 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( 0xFF & ( size >> 0 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( pps[i].data() );
    }

    return sequenceHeader;
}

bool VideoEncoder::encode(const QImage &image)
{
    Q_ASSERT(image.size() == QSize(param.i_width, param.i_height));
    Q_ASSERT(image.format() == QImage::Format_RGB888);

    convertRGB888toYUV420P(image.bits(), pic_in.img.plane,
                        param.i_width, param.i_height);

    nals_size_ = x264_encoder_encode(encoder, &nals, &i_nals, &pic_in, &pic_out);
    if(nals_size_ < 0) return false;

    return true;
}

/*
bool VideoEncoder::prepareHeader()
{
    //nals_size_ = x264_encoder_headers(encoder, &nals, &i_nals);
    //qDebug() << "x264 headers size" << nals_size_;
    //if(nals_size_ < 0)  return false;

    x264_encoder_headers( m_264enc, &nals, &i_nals );
    QVector<QByteArray> sps, pps;
    for(int i = 0 ; i < i_nals ; ++i )
    {
        switch( nals[i].i_type )
        { // +4, -4, removes the 4 byte size at teh begining for the NALU
        case 7: sps.append( QByteArray((const char*)nals[i].p_payload + 4, nals[i].i_payload - 4) ); break;
        case 8: pps.append( QByteArray((const char*)nals[i].p_payload + 4, nals[i].i_payload - 4) ); break;
        }
    }

    // TODO make sure we have sps and pps
    QByteArray sequenceHeader;
    sequenceHeader.append( 0x01 );  // version
    sequenceHeader.append( sps[0][1] ); // profile
    sequenceHeader.append( sps[0][2] ); // compatibility
    sequenceHeader.append( sps[0][3] ); // level
    sequenceHeader.append(0xFC | 3); // reserved (6 bits), NULA length size - 1 (2 bits)
    sequenceHeader.append(0xE0 | sps.size() ); // reserved (3 bits), num of SPS (5 bits)
    for(int i = 0 ; i < sps.size() ; ++i )
    {
        quint16 size = sps[i].size();
        sequenceHeader.append( 0xFF & ( size >> 8 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( 0xFF & ( size >> 0 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( sps[i] );
    }

    sequenceHeader.append( pps.size() );
    for(int i = 0 ; i < pps.size() ; ++i )
    {
        quint16 size = pps[i].size();
        sequenceHeader.append( 0xFF & ( size >> 8 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( 0xFF & ( size >> 0 ) ); // 2 bytes for length of SPS
        sequenceHeader.append( pps[i].data() );
    }

    return true;
}

bool VideoEncoder::encode(const QImage &image)
{
    Q_ASSERT(image.size() == QSize(param.i_width, param.i_height));
    Q_ASSERT(image.format() == QImage::Format_RGB888);

    convertRGB888toYUV420P(image.bits(), pic_in.img.plane,
                        param.i_width, param.i_height);

    nals_size_ = x264_encoder_encode(encoder, &nals, &i_nals, &pic_in, &pic_out);
    if(nals_size_ < 0) return false;

    return true;
}
*/

int VideoEncoder::getFrameSize() const
{
    return nals_size_;
}

bool VideoEncoder::isKeyFrame() const
{
    return pic_out.b_keyframe;
}

const char* VideoEncoder::getData()
{
    return (const char*)nals[0].p_payload;
}
