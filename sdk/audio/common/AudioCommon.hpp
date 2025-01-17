#pragma once

#include <stdint.h>

enum AudioFormat {
    AudioFormatUnknown = -1,
    AudioFormatS8,
    AudioFormatU8,
    AudioFormatS16,
    AudioFormatS16BE,
    AudioFormatU16,
    AudioFormatU16BE,
    AudioFormatS24,
    AudioFormatS24BE,
    AudioFormatU24,
    AudioFormatU24BE,
    AudioFormatS32,
    AudioFormatS32BE,
    AudioFormatFLT32,
    AudioFormatFLT32BE,
    AudioFormatDBL64,
    AudioFormatDBL64BE,
};

inline AudioFormat getAudioFormatByBitPreSample(int bits)
{
    switch (bits) {
    case 8:
        return AudioFormatU8;
    case 16:
        return AudioFormatS16;
    case 24:
        return AudioFormatS24;
    case 32:
        return AudioFormatS32;
    case 64:
        return AudioFormatDBL64;
    default:
        return AudioFormatUnknown;
    }
}

inline int getBytePreSampleByAudioFormat(AudioFormat format)
{
    switch (format) {
    case AudioFormatS8:
    case AudioFormatU8:
        return 1;
    case AudioFormatS16:
    case AudioFormatS16BE:
    case AudioFormatU16:
    case AudioFormatU16BE:
        return 2;
    case AudioFormatS24:
    case AudioFormatS24BE:
    case AudioFormatU24:
    case AudioFormatU24BE:
        return 3;
    case AudioFormatS32:
    case AudioFormatS32BE:
    case AudioFormatFLT32:
    case AudioFormatFLT32BE:
        return 4;
    case AudioFormatDBL64:
    case AudioFormatDBL64BE:
        return 8;
    default:
        return 0;
    }
}

struct AudioSpec {
    int sampleRate;
    AudioFormat format;
    int numChannel;
    int bitsPerSample;
    int bytesPerSample;
    uint64_t samples;
    uint64_t durationMs;

    AudioSpec()
        : sampleRate(0)
        , format(AudioFormatUnknown)
        , numChannel(0)
        , bitsPerSample(0)
        , bytesPerSample(0)
        , samples(0)
        , durationMs(0)
    {
    }

    bool operator==(const AudioSpec &other) const
    {
        return sampleRate == other.sampleRate && format == other.format
            && numChannel == other.numChannel;
    }
};

enum AudioCodecID {
    AUDIO_CODEC_ID_NONE = -1,
    AUDIO_CODEC_ID_MP2  = 0x15000,
    AUDIO_CODEC_ID_MP3,
    AUDIO_CODEC_ID_AAC,
    AUDIO_CODEC_ID_AC3,
    AUDIO_CODEC_ID_DTS,
    AUDIO_CODEC_ID_VORBIS,
    AUDIO_CODEC_ID_DVAUDIO,
    AUDIO_CODEC_ID_WMAV1,
    AUDIO_CODEC_ID_WMAV2,
    AUDIO_CODEC_ID_MACE3,
    AUDIO_CODEC_ID_MACE6,
    AUDIO_CODEC_ID_VMDAUDIO,
    AUDIO_CODEC_ID_FLAC,
    AUDIO_CODEC_ID_MP3ADU,
    AUDIO_CODEC_ID_MP3ON4,
    AUDIO_CODEC_ID_SHORTEN,
    AUDIO_CODEC_ID_ALAC,
    AUDIO_CODEC_ID_WESTWOOD_SND1,
    AUDIO_CODEC_ID_GSM, ///< as in Berlin toast format
    AUDIO_CODEC_ID_QDM2,
    AUDIO_CODEC_ID_COOK,
    AUDIO_CODEC_ID_TRUESPEECH,
    AUDIO_CODEC_ID_TTA,
    AUDIO_CODEC_ID_SMACKAUDIO,
    AUDIO_CODEC_ID_QCELP,
    AUDIO_CODEC_ID_WAVPACK,
    AUDIO_CODEC_ID_DSICINAUDIO,
    AUDIO_CODEC_ID_IMC,
    AUDIO_CODEC_ID_MUSEPACK7,
    AUDIO_CODEC_ID_MLP,
    AUDIO_CODEC_ID_GSM_MS, /* as found in WAV */
    AUDIO_CODEC_ID_ATRAC3,
    AUDIO_CODEC_ID_APE,
    AUDIO_CODEC_ID_NELLYMOSER,
    AUDIO_CODEC_ID_MUSEPACK8,
    AUDIO_CODEC_ID_SPEEX,
    AUDIO_CODEC_ID_WMAVOICE,
    AUDIO_CODEC_ID_WMAPRO,
    AUDIO_CODEC_ID_WMALOSSLESS,
    AUDIO_CODEC_ID_ATRAC3P,
    AUDIO_CODEC_ID_EAC3,
    AUDIO_CODEC_ID_SIPR,
    AUDIO_CODEC_ID_MP1,
    AUDIO_CODEC_ID_TWINVQ,
    AUDIO_CODEC_ID_TRUEHD,
    AUDIO_CODEC_ID_MP4ALS,
    AUDIO_CODEC_ID_ATRAC1,
    AUDIO_CODEC_ID_BINKAUDIO_RDFT,
    AUDIO_CODEC_ID_BINKAUDIO_DCT,
    AUDIO_CODEC_ID_AAC_LATM,
    AUDIO_CODEC_ID_QDMC,
    AUDIO_CODEC_ID_CELT,
    AUDIO_CODEC_ID_G723_1,
    AUDIO_CODEC_ID_G729,
    AUDIO_CODEC_ID_8SVX_EXP,
    AUDIO_CODEC_ID_8SVX_FIB,
    AUDIO_CODEC_ID_BMV_AUDIO,
    AUDIO_CODEC_ID_RALF,
    AUDIO_CODEC_ID_IAC,
    AUDIO_CODEC_ID_ILBC,
    AUDIO_CODEC_ID_OPUS,
    AUDIO_CODEC_ID_COMFORT_NOISE,
    AUDIO_CODEC_ID_TAK,
    AUDIO_CODEC_ID_METASOUND,
    AUDIO_CODEC_ID_PAF_AUDIO,
    AUDIO_CODEC_ID_ON2AVC,
    AUDIO_CODEC_ID_DSS_SP,
    AUDIO_CODEC_ID_CODEC2,
    AUDIO_CODEC_ID_FFWAVESYNTH = 0x15800,
    AUDIO_CODEC_ID_SONIC,
    AUDIO_CODEC_ID_SONIC_LS,
    AUDIO_CODEC_ID_EVRC,
    AUDIO_CODEC_ID_SMV,
    AUDIO_CODEC_ID_DSD_LSBF,
    AUDIO_CODEC_ID_DSD_MSBF,
    AUDIO_CODEC_ID_DSD_LSBF_PLANAR,
    AUDIO_CODEC_ID_DSD_MSBF_PLANAR,
    AUDIO_CODEC_ID_4GV,
    AUDIO_CODEC_ID_INTERPLAY_ACM,
    AUDIO_CODEC_ID_XMA1,
    AUDIO_CODEC_ID_XMA2,
    AUDIO_CODEC_ID_DST,
    AUDIO_CODEC_ID_ATRAC3AL,
    AUDIO_CODEC_ID_ATRAC3PAL,
    AUDIO_CODEC_ID_DOLBY_E,
    AUDIO_CODEC_ID_APTX,
    AUDIO_CODEC_ID_APTX_HD,
    AUDIO_CODEC_ID_SBC,
    AUDIO_CODEC_ID_ATRAC9,
    AUDIO_CODEC_ID_HCOM,
    AUDIO_CODEC_ID_ACELP_KELVIN,
    AUDIO_CODEC_ID_MPEGH_3D_AUDIO,
    AUDIO_CODEC_ID_SIREN,
    AUDIO_CODEC_ID_HCA,
};