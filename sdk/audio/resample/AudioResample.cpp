#include "AudioResample.h"
#include "LogWrapper.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
#include <iostream>

#define LOG_TAG "AudioResample"

struct AudioResampleSpec {
    int channelNum;
    int channelLayout;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int bytesPerSample;
    int samples;
    int lineSize;
};

class AudioResample::Impl {
private:
    bool m_isInit;
    int m_isAlign;
    AudioSpec m_in;
    AudioSpec m_out;
    uint8_t **m_inData;
    uint8_t **m_outData;
    SwrContext *m_swrCtx;
    AudioResampleSpec m_inSpec;
    AudioResampleSpec m_outSpec;
    char m_error[1024];
    sdk_utils::status_t m_initCheck;

public:
    Impl();
    ~Impl();
    void init(AudioSpec &in, AudioSpec &out);
    sdk_utils::status_t initCheck() { return m_initCheck; }
    int resample(void *in, size_t inLen, void *out, size_t *outLen);

private:
    bool AudioSpec2AudioResampleSpec(AudioSpec &in, AudioResampleSpec &out);
    void getAVErrText(int err, char *errText, int errTextSize);
    void printAudioSpec(AudioResampleSpec &spec);
};

AudioResample::Impl::Impl()
    : m_isInit(false)
    , m_isAlign(1)
    , m_in()
    , m_out()
    , m_inData(nullptr)
    , m_outData(nullptr)
    , m_swrCtx(nullptr)
    , m_inSpec()
    , m_outSpec()
    , m_initCheck(sdk_utils::UNKNOWN_ERROR)
{
    LOG_INFO(LOG_TAG, "Impl::Impl()");
}

AudioResample::Impl::~Impl()
{
    LOG_INFO(LOG_TAG, "enter");
    if (m_inData) {
        av_freep(&m_inData[0]);
        av_freep(&m_inData);
    }
    if (m_outData) {
        av_freep(&m_outData[0]);
        av_freep(&m_outData);
    }
    if (m_swrCtx) {
        swr_free(&m_swrCtx);
    }
}

void AudioResample::Impl::init(AudioSpec &in, AudioSpec &out)
{
    LOG_INFO(LOG_TAG, "enter");
    if (m_isInit) {
        LOG_WARNING(LOG_TAG, "already init");
        return;
    }
    LOG_INFO(LOG_TAG, "av log level: %d", av_log_get_level());
    av_log_set_level(AV_LOG_TRACE);
    LOG_INFO(LOG_TAG, "av log level: %d", av_log_get_level());
    av_log_set_callback([](void *avcl, int level, const char *fmt, va_list vl) {
        char line[1024];
        vsnprintf(line, sizeof(line), fmt, vl);
        switch (level) {
        case AV_LOG_INFO:
            LOG_INFO(LOG_TAG, "%s", line);
            break;
        case AV_LOG_WARNING:
            LOG_WARNING(LOG_TAG, "%s", line);
            break;
        case AV_LOG_ERROR:
            LOG_ERROR(LOG_TAG, "%s", line);
            break;
        case AV_LOG_FATAL:
            LOG_FATAL(LOG_TAG, "%s", line);
            break;
        default:
            break;
        }
    });
    m_in = in;
    m_out = out;
    LOGD("in  spec %d %d %d", m_in.sampleRate, m_in.numChannel, m_in.format);
    LOGD("out spec %d %d %d", m_out.sampleRate, m_out.numChannel, m_out.format);
    if(!AudioSpec2AudioResampleSpec(m_in, m_inSpec)) {
        LOG_ERROR(LOG_TAG, "AudioSpec2AudioResampleSpec input failed");
        return;
    }
    printAudioSpec(m_inSpec);
    m_out.samples = m_inSpec.samples * m_out.sampleRate / m_inSpec.sampleRate;
    if(!AudioSpec2AudioResampleSpec(m_out, m_outSpec)) {
        LOG_ERROR(LOG_TAG, "AudioSpec2AudioResampleSpec output failed");
        return;
    }
    printAudioSpec(m_outSpec);
    m_swrCtx = swr_alloc_set_opts(nullptr, m_outSpec.channelLayout, m_outSpec.sampleFmt,
        m_outSpec.sampleRate, m_inSpec.channelLayout, m_inSpec.sampleFmt, m_inSpec.sampleRate, 0,
        nullptr);
    if (!m_swrCtx) {
        LOG_WARNING(LOG_TAG, "swr_alloc_set_opts failed");
        return;
    }
    int ret = swr_init(m_swrCtx);
    if (ret < 0) {
        getAVErrText(ret, m_error, sizeof(m_error));
        LOG_ERROR(LOG_TAG, "swr_init failed: %s", m_error);
        return;
    }
    m_inData = (uint8_t **)av_malloc(sizeof(uint8_t *) * m_in.numChannel);
    m_outData = (uint8_t **)av_malloc(sizeof(uint8_t *) * m_out.numChannel);
    ret = av_samples_alloc(m_inData, &m_inSpec.lineSize, m_in.numChannel, m_inSpec.samples, m_inSpec.sampleFmt, m_isAlign);
    if(ret <= 0) {
        getAVErrText(ret, m_error, sizeof(m_error));
        LOG_ERROR(LOG_TAG, "in av_samples_alloc failed: %s", m_error);
        return;
    }
    LOG_INFO(LOG_TAG, "ret: %d, m_inSpec.lineSize: %d", ret, m_inSpec.lineSize);
    ret = av_samples_alloc(m_outData, &m_outSpec.lineSize, m_out.numChannel, m_outSpec.samples, m_outSpec.sampleFmt, m_isAlign);
    if(ret <= 0) {
        getAVErrText(ret, m_error, sizeof(m_error));
        LOG_ERROR(LOG_TAG, "out av_samples_alloc failed: %s", m_error);
        return;
    }
    LOG_INFO(LOG_TAG, "ret: %d, m_outSpec.lineSize: %d", ret, m_outSpec.lineSize);
    m_initCheck = sdk_utils::OK;
    m_isInit = true;
    LOG_INFO(LOG_TAG, "exit");
}

int AudioResample::Impl::resample(void *in, size_t inLen, void *out, size_t *outLen)
{
    if(inLen <= 0 || outLen <= 0) {
        LOG_WARNING(LOG_TAG, "inLen or outLen is invalid");
        return -1;
    }
    if(!m_isInit) {
        LOG_WARNING(LOG_TAG, "not init");
        return -1;
    }
    if(inLen > (size_t)m_inSpec.lineSize) {
        LOG_WARNING(LOG_TAG, "inLen is too large, inLen: %d, lineSize: %d", inLen, m_inSpec.lineSize);
        return -3;
    } else if(inLen < (size_t)m_inSpec.lineSize) {
        m_inSpec.samples = inLen / m_inSpec.bytesPerSample;
        memset(m_inData[0], 0, m_inSpec.lineSize);
        memcpy(m_inData[0], in, inLen);
    } else {
        memcpy(m_inData[0], in, inLen);
    }
    // LOG_DEBUG(LOG_TAG, "inLen: %d, in_samples: %d", inLen, m_inSpec.samples);
    int ret = swr_convert(m_swrCtx, m_outData, m_outSpec.samples, (const uint8_t **)m_inData, m_inSpec.samples);
    if(ret < 0) {
        getAVErrText(ret, m_error, sizeof(m_error));
        LOG_ERROR(LOG_TAG, "swr_convert failed: %s", m_error);
        return ret;
    }
    int outSize = av_samples_get_buffer_size(nullptr, m_out.numChannel, ret, m_outSpec.sampleFmt, m_isAlign);
    if(outSize <= 0) {
        LOG_FATAL(LOG_TAG, "av_samples_get_buffer_size failed, outSize: %d", outSize);
        return -4;
    }
    memcpy(out, m_outData[0], outSize);
    *outLen = outSize;
    return 0;
}

bool AudioResample::Impl::AudioSpec2AudioResampleSpec(AudioSpec &in, AudioResampleSpec &out)
{
    bool ret = true;
    out.channelLayout = av_get_default_channel_layout(in.numChannel);
    out.channelNum = in.numChannel;
    out.sampleRate = in.sampleRate;
    out.samples = in.samples;
    switch (in.format)
    {
    case AudioFormatU8:
    case AudioFormatS8:
        out.sampleFmt = AV_SAMPLE_FMT_U8;
        break;
    case AudioFormatU16:
    case AudioFormatS16:
    case AudioFormatU16BE:
    case AudioFormatS16BE:
        out.sampleFmt = AV_SAMPLE_FMT_S16;
        break;
    case AudioFormatS32:
    case AudioFormatS32BE:
        out.sampleFmt = AV_SAMPLE_FMT_S32;
        break;
    case AudioFormatFLT32:
    case AudioFormatFLT32BE:
        out.sampleFmt = AV_SAMPLE_FMT_FLT;
        break;
    case AudioFormatDBL64:
    case AudioFormatDBL64BE:
        out.sampleFmt = AV_SAMPLE_FMT_DBL;
        break;
    default:
        out.sampleFmt = AV_SAMPLE_FMT_NONE;
        LOG_ERROR(LOG_TAG, "unsupport format: %d", in.format);
        ret = false;
        break;
    }
    out.bytesPerSample = in.numChannel * av_get_bytes_per_sample(out.sampleFmt);
    return ret;
}

void AudioResample::Impl::getAVErrText(int err, char *errText, int errTextSize) {
    av_strerror(err, errText, errTextSize);
 }

 void AudioResample::Impl::printAudioSpec(AudioResampleSpec &spec) {
    LOG_INFO(LOG_TAG, "chL:%d, ch:%d, Rate:%d, samples:%d, Fmt:%d, bytesPerSample:%d",
        spec.channelLayout, spec.channelNum, spec.sampleRate, spec.samples, spec.sampleFmt, spec.bytesPerSample);
  }

 AudioResample::AudioResample()
     : m_impl(new AudioResample::Impl())
 {
}

AudioResample::AudioResample(AudioSpec &in, AudioSpec &out)
    : AudioResample()
{
    m_impl->init(in, out);
}

AudioResample::~AudioResample() { }

void AudioResample::init(AudioSpec &in, AudioSpec &out)
{
    m_impl->init(in, out);
}

sdk_utils::status_t AudioResample::initCheck()
{
    return m_impl->initCheck();
}

int AudioResample::resample(void *in, size_t inLen, void *out, size_t *outLen)
{
    return m_impl->resample(in, inLen, out, outLen);
}
