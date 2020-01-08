/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "QtiGralloc.h"

#include <log/log.h>
namespace qtigralloc {

using android::hardware::graphics::mapper::V4_0::IMapper;
static sp<IMapper> getInstance() {
  static sp<IMapper> mapper = IMapper::getService();
  return mapper;
}
Error decodeColorMetadata(hidl_vec<uint8_t> &in, ColorMetaData *out) {
  if (!in.size() || !out) {
    return Error::BAD_VALUE;
  }
  memcpy(out, in.data(), sizeof(ColorMetaData));
  return Error::NONE;
}

Error encodeColorMetadata(ColorMetaData &in, hidl_vec<uint8_t> *out) {
  if (!out) {
    return Error::BAD_VALUE;
  }
  out->resize(sizeof(ColorMetaData));
  memcpy(out->data(), &in, sizeof(ColorMetaData));
  return Error::NONE;
}

Error decodeGraphicsMetadata(hidl_vec<uint8_t> &in, GraphicsMetadata *out) {
  if (!in.size() || !out) {
    return Error::BAD_VALUE;
  }
  memcpy(out, in.data(), sizeof(GraphicsMetadata));
  return Error::NONE;
}

Error encodeGraphicsMetadata(GraphicsMetadata &in, hidl_vec<uint8_t> *out) {
  if (!out) {
    return Error::BAD_VALUE;
  }
  out->resize(sizeof(GraphicsMetadata));
  memcpy(out->data(), &in, sizeof(GraphicsMetadata));
  return Error::NONE;
}

Error decodeUBWCStats(hidl_vec<uint8_t> &in, UBWCStats *out) {
  if (!in.size() || !out) {
    return Error::BAD_VALUE;
  }
  memcpy(out, in.data(), UBWC_STATS_ARRAY_SIZE * sizeof(UBWCStats));
  return Error::NONE;
}

Error encodeUBWCStats(UBWCStats *in, hidl_vec<uint8_t> *out) {
  if (!in || !out) {
    return Error::BAD_VALUE;
  }
  out->resize(UBWC_STATS_ARRAY_SIZE * sizeof(UBWCStats));
  memcpy(out->data(), &in[0], UBWC_STATS_ARRAY_SIZE * sizeof(UBWCStats));
  return Error::NONE;
}

Error decodeCVPMetadata(hidl_vec<uint8_t> &in, CVPMetadata *out) {
  if (!in.size() || !out) {
    return Error::BAD_VALUE;
  }
  memcpy(out, in.data(), sizeof(CVPMetadata));
  return Error::NONE;
}

Error encodeCVPMetadata(CVPMetadata &in, hidl_vec<uint8_t> *out) {
  if (!out) {
    return Error::BAD_VALUE;
  }
  out->resize(sizeof(CVPMetadata));
  memcpy(out->data(), &in, sizeof(CVPMetadata));
  return Error::NONE;
}

Error decodeVideoHistogramMetadata(hidl_vec<uint8_t> &in, VideoHistogramMetadata *out) {
  if (!in.size() || !out) {
    return Error::BAD_VALUE;
  }
  memcpy(out, in.data(), sizeof(VideoHistogramMetadata));
  return Error::NONE;
}

Error encodeVideoHistogramMetadata(VideoHistogramMetadata &in, hidl_vec<uint8_t> *out) {
  if (!out) {
    return Error::BAD_VALUE;
  }
  out->resize(sizeof(VideoHistogramMetadata));
  memcpy(out->data(), &in, sizeof(VideoHistogramMetadata));
  return Error::NONE;
}

MetadataType getMetadataType(uint32_t in) {
  switch (in) {
    case QTI_VT_TIMESTAMP:
      return MetadataType_VTTimestamp;
    case QTI_VIDEO_PERF_MODE:
      return MetadataType_VideoPerfMode;
    case QTI_LINEAR_FORMAT:
      return MetadataType_LinearFormat;
    case QTI_SINGLE_BUFFER_MODE:
      return MetadataType_SingleBufferMode;
    case QTI_PP_PARAM_INTERLACED:
      return MetadataType_PPParamInterlaced;
    case QTI_MAP_SECURE_BUFFER:
      return MetadataType_MapSecureBuffer;
    case QTI_COLOR_METADATA:
      return MetadataType_ColorMetadata;
    case QTI_GRAPHICS_METADATA:
      return MetadataType_GraphicsMetadata;
    case QTI_UBWC_CR_STATS_INFO:
      return MetadataType_UBWCCRStatsInfo;
    case QTI_REFRESH_RATE:
      return MetadataType_RefreshRate;
    case QTI_CVP_METADATA:
      return MetadataType_CVPMetadata;
    case QTI_VIDEO_HISTOGRAM_STATS:
      return MetadataType_VideoHistogramStats;
    case QTI_FD:
      return MetadataType_FD;
    case QTI_PRIVATE_FLAGS:
      return MetadataType_PrivateFlags;
    default:
      return MetadataType_Invalid;
  }
}

Error get(void *buffer, uint32_t type, void *param) {
  hidl_vec<uint8_t> bytestream;
  sp<IMapper> mapper = getInstance();

  MetadataType metadata_type = getMetadataType(type);
  if (metadata_type == MetadataType_Invalid) {
    param = nullptr;
    return Error::UNSUPPORTED;
  }

  auto err = Error::UNSUPPORTED;
  mapper->get(buffer, metadata_type, [&](const auto &tmpError, const auto &tmpByteStream) {
    err = tmpError;
    bytestream = tmpByteStream;
  });

  if (err != Error::NONE) {
    return err;
  }

  switch (type) {
    case QTI_VT_TIMESTAMP:
      err = static_cast<Error>(android::gralloc4::decodeUint64(qtigralloc::MetadataType_VTTimestamp,
                                                               bytestream, (uint64_t *)param));
      break;
    case QTI_VIDEO_PERF_MODE:
      err = static_cast<Error>(android::gralloc4::decodeUint32(
          qtigralloc::MetadataType_VideoPerfMode, bytestream, (uint32_t *)param));
      break;
    case QTI_LINEAR_FORMAT:
      err = static_cast<Error>(android::gralloc4::decodeUint32(
          qtigralloc::MetadataType_LinearFormat, bytestream, (uint32_t *)param));
      break;
    case QTI_SINGLE_BUFFER_MODE:
      err = static_cast<Error>(android::gralloc4::decodeUint32(
          qtigralloc::MetadataType_SingleBufferMode, bytestream, (uint32_t *)param));
      break;
    case QTI_PP_PARAM_INTERLACED:
      err = static_cast<Error>(android::gralloc4::decodeInt32(
          qtigralloc::MetadataType_PPParamInterlaced, bytestream, (int32_t *)param));
      break;
    case QTI_MAP_SECURE_BUFFER:
      err = static_cast<Error>(android::gralloc4::decodeInt32(
          qtigralloc::MetadataType_MapSecureBuffer, bytestream, (int32_t *)param));
      break;
    case QTI_COLOR_METADATA:
      err = decodeColorMetadata(bytestream, (ColorMetaData *)param);
      break;
    case QTI_GRAPHICS_METADATA:
      err = decodeGraphicsMetadata(bytestream, (GraphicsMetadata *)param);
      break;
    case QTI_UBWC_CR_STATS_INFO:
      err = decodeUBWCStats(bytestream, (UBWCStats *)param);
      break;
    case QTI_REFRESH_RATE:
      err = static_cast<Error>(android::gralloc4::decodeFloat(qtigralloc::MetadataType_RefreshRate,
                                                              bytestream, (float *)param));
      break;
    case QTI_CVP_METADATA:
      err = decodeCVPMetadata(bytestream, (CVPMetadata *)param);
      break;
    case QTI_VIDEO_HISTOGRAM_STATS:
      err = decodeVideoHistogramMetadata(bytestream, (VideoHistogramMetadata *)param);
      break;
    case QTI_FD:
      err = static_cast<Error>(android::gralloc4::decodeInt32(qtigralloc::MetadataType_FD,
                                                              bytestream, (int32_t *)param));
      break;
    case QTI_PRIVATE_FLAGS:
      err = static_cast<Error>(android::gralloc4::decodeInt32(qtigralloc::MetadataType_PrivateFlags,
                                                              bytestream, (int32_t *)param));
      break;
    default:
      param = nullptr;
      return Error::UNSUPPORTED;
  }

  return err;
}

Error set(void *buffer, uint32_t type, void *param) {
  hidl_vec<uint8_t> bytestream;
  sp<IMapper> mapper = getInstance();

  Error err = Error::UNSUPPORTED;
  MetadataType metadata_type = getMetadataType(type);
  if (metadata_type == MetadataType_Invalid) {
    return err;
  }

  switch (type) {
    case QTI_VT_TIMESTAMP:
      err = static_cast<Error>(android::gralloc4::encodeUint64(qtigralloc::MetadataType_VTTimestamp,
                                                               *(uint64_t *)param, &bytestream));
      break;
    case QTI_VIDEO_PERF_MODE:
      err = static_cast<Error>(android::gralloc4::encodeUint32(
          qtigralloc::MetadataType_VideoPerfMode, *(uint32_t *)param, &bytestream));
      break;
    case QTI_LINEAR_FORMAT:
      err = static_cast<Error>(android::gralloc4::encodeUint32(
          qtigralloc::MetadataType_LinearFormat, *(uint32_t *)param, &bytestream));
      break;
    case QTI_SINGLE_BUFFER_MODE:
      err = static_cast<Error>(android::gralloc4::encodeUint32(
          qtigralloc::MetadataType_SingleBufferMode, *(uint32_t *)param, &bytestream));
      break;
    case QTI_PP_PARAM_INTERLACED:
      err = static_cast<Error>(android::gralloc4::encodeInt32(
          qtigralloc::MetadataType_PPParamInterlaced, *(int32_t *)param, &bytestream));
      break;
    case QTI_MAP_SECURE_BUFFER:
      err = static_cast<Error>(android::gralloc4::encodeInt32(
          qtigralloc::MetadataType_MapSecureBuffer, *(int32_t *)param, &bytestream));
      break;
    case QTI_COLOR_METADATA:
      err = encodeColorMetadata(*(ColorMetaData *)param, &bytestream);
      break;
    case QTI_GRAPHICS_METADATA:
      err = encodeGraphicsMetadata(*(GraphicsMetadata *)param, &bytestream);
      break;
    case QTI_UBWC_CR_STATS_INFO:
      err = encodeUBWCStats((UBWCStats *)param, &bytestream);
      break;
    case QTI_REFRESH_RATE:
      err = static_cast<Error>(android::gralloc4::encodeFloat(qtigralloc::MetadataType_RefreshRate,
                                                              *(float *)param, &bytestream));
      break;
    case QTI_CVP_METADATA:
      err = encodeCVPMetadata(*(CVPMetadata *)param, &bytestream);
      break;
    case QTI_VIDEO_HISTOGRAM_STATS:
      err = encodeVideoHistogramMetadata(*(VideoHistogramMetadata *)param, &bytestream);
      break;
    default:
      param = nullptr;
      return Error::UNSUPPORTED;
  }

  if (err != Error::NONE) {
    return err;
  }

  return mapper->set((void *)buffer, metadata_type, bytestream);
}

}  // namespace qtigralloc
