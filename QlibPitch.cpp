#include "QlibPitch.h"

namespace q = cycfi::q;
using namespace q::literals;
using std::fixed;
QlibPitch::QlibPitch(float inputSampleRate) :
    Plugin(inputSampleRate),
        // m_pd is initialized first so we provide default args here as well
        m_pd(std::make_shared<cycfi::q::pitch_detector>(100_Hz, 800_Hz, inputSampleRate, -45_dB)),
        m_blockSize(0),
        m_highestPitch(800.0),
        m_lowestPitch(100.0),
        m_threshold(-45.0),
        m_frequency(0.0),
        m_regularOutput(false),
        m_regularOutputStep(128)
{
}

QlibPitch::~QlibPitch()
{
}

string
QlibPitch::getIdentifier() const
{
    return "qlib_pitch";
}

string
QlibPitch::getName() const
{
    return "Qlib Pitch Detector";
}

string
QlibPitch::getDescription() const
{
    return "Track pitch using bitstream autocorrelation";
}

string
QlibPitch::getMaker() const
{
    // Your name here
    return "Cycfi research (plugin by Xavier Riley)";
}

int
QlibPitch::getPluginVersion() const
{
    // Increment this each time you release a version that behaves
    // differently from the previous one
    return 1;
}

string
QlibPitch::getCopyright() const
{
    // This function is not ideally named.  It does not necessarily
    // need to say who made the plugin -- getMaker does that -- but it
    // should indicate the terms under which it is distributed.  For
    // example, "Copyright (year). All Rights Reserved", or "GPL"
    return "MIT";
}

QlibPitch::InputDomain
QlibPitch::getInputDomain() const
{
    return TimeDomain;
}

size_t
QlibPitch::getPreferredBlockSize() const
{
    return 0; // 0 means "I can handle any block size"
}

size_t 
QlibPitch::getPreferredStepSize() const
{
    return 0; // 0 means "anything sensible"; in practice this
              // means the same as the block size for TimeDomain
              // plugins, or half of it for FrequencyDomain plugins
}

size_t
QlibPitch::getMinChannelCount() const
{
    return 1;
}

size_t
QlibPitch::getMaxChannelCount() const
{
    return 1;
}

QlibPitch::ParameterList
QlibPitch::getParameterDescriptors() const
{
    ParameterList list;

    ParameterDescriptor lowestPitch;
    lowestPitch.identifier = "lowestPitch";
    lowestPitch.name = "Lowest Pitch";
    lowestPitch.description = "Lower limit for pitch tracking output. Defaults to 100Hz";
    lowestPitch.unit = "Hz";
    lowestPitch.minValue = 20;
    lowestPitch.maxValue = 2000;
    lowestPitch.defaultValue = 100.0;
    lowestPitch.isQuantized = false;
    list.push_back(lowestPitch);

    ParameterDescriptor highestPitch;
    highestPitch.identifier = "highestPitch";
    highestPitch.name = "Highest Pitch";
    highestPitch.description = "Upper limit for pitch tracking output. Defaults to 800Hz";
    highestPitch.unit = "Hz";
    highestPitch.minValue = 20;
    highestPitch.maxValue = 5000;
    highestPitch.defaultValue = 800.0;
    highestPitch.isQuantized = false;
    list.push_back(highestPitch);

    ParameterDescriptor threshold;
    threshold.identifier = "threshold";
    threshold.name = "Hysterisis Threshold";
    threshold.description = "Pitches will not be given for signals below this volume level.";
    threshold.unit = "dB";
    threshold.minValue = -100.0;
    threshold.maxValue = 0.0;
    threshold.defaultValue = -45.0;
    threshold.isQuantized = false;
    list.push_back(threshold);

    ParameterDescriptor regularOutput;
    regularOutput.identifier = "regularOutput";
    regularOutput.name = "Regular Output";
    regularOutput.description = "If set to 1, output from the plugin is at regular intervals of n-frames, as defined by the Regular Output Step param. This can help to line up detected pitches with some datasets.";
    regularOutput.minValue = 0;
    regularOutput.maxValue = 1;
    regularOutput.defaultValue = 0;
    regularOutput.isQuantized = true;
    regularOutput.quantizeStep = 1;
    list.push_back(regularOutput);

    ParameterDescriptor regularOutputStep;
    regularOutputStep.identifier = "regularOutputStep";
    regularOutputStep.name = "Regular Output";
    regularOutputStep.description = "Number of frames to use for the interval when Regular Output is enabled. Must not exceed block size.";
    regularOutputStep.minValue = 0;
    regularOutputStep.maxValue = 4096;
    regularOutputStep.defaultValue = 128.0;
    regularOutputStep.isQuantized = true;
    regularOutputStep.quantizeStep = 1;
    list.push_back(regularOutputStep);

    return list;
}

float
QlibPitch::getParameter(string identifier) const
{
    if (identifier == "lowestPitch") {
        return static_cast<float>(m_lowestPitch);
    } else if (identifier == "highestPitch") {
        return static_cast<float>(m_highestPitch);
    } else if (identifier == "threshold") {
        return static_cast<float>(m_threshold);
    } else if (identifier == "regularOutput") {
        return static_cast<float>(m_regularOutput);
    } else if (identifier == "regularOutputStep") {
        return static_cast<float>(m_regularOutputStep);
    } else {
        return 0;
    }
}

void
QlibPitch::setParameter(string identifier, float value) 
{
    if (identifier == "lowestPitch") {
        m_lowestPitch = value;
    } else if (identifier == "highestPitch") {
        m_highestPitch = value;
    } else if (identifier == "threshold") {
        m_threshold = value;
    } else if (identifier == "regularOutput") {
        m_regularOutput = int(value);
    } else if (identifier == "regularOutputStep") {
        m_regularOutputStep = int(value);
    }
}

QlibPitch::ProgramList
QlibPitch::getPrograms() const
{
    ProgramList list;

    // If you have no programs, return an empty list (or simply don't
    // implement this function or getCurrentProgram/selectProgram)

    return list;
}

string
QlibPitch::getCurrentProgram() const
{
    return ""; // no programs
}

void
QlibPitch::selectProgram(string name)
{
}

QlibPitch::OutputList
QlibPitch::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.

    OutputDescriptor d;
    d.identifier = "frequency";
    d.name = "Fundamental Frequency";
    d.description = "List of detected frequencies";
    d.unit = "Hz";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::VariableSampleRate;
    d.hasDuration = false;
    list.push_back(d);

    return list;
}

bool
QlibPitch::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
        channels > getMaxChannelCount()) return false;

    // Real initialisation work goes here!
    m_blockSize = blockSize;

    m_pd = std::make_shared<cycfi::q::pitch_detector>(m_lowestPitch, m_highestPitch, m_inputSampleRate, q::decibel(q::detail::db2a(m_threshold)));

    return true;
}

void
QlibPitch::reset()
{
    // Clear buffers, reset stored values, etc
}

QlibPitch::FeatureSet
QlibPitch::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{
    Vamp::RealTime blockStartTimestamp = timestamp;
    size_t currentFrameWithinBlock = 0;
    size_t pitchesWithinBlock = 0;
    FeatureSet fs;

    uint32_t sps = static_cast<uint32_t>(m_inputSampleRate);

    constexpr float            slope = 1.0f/4;
    constexpr float            makeup_gain = 4;
    q::compressor              comp{ -18_dB, slope };
    q::clip                    clip;

    float                      onset_threshold = float(-28_dB);
    float                      release_threshold = float(-60_dB);
    float                      threshold = onset_threshold;
    q::peak_envelope_follower  env{ 30_ms, sps };
    q::one_pole_lowpass        lp{ m_highestPitch, sps };
    q::one_pole_lowpass        lp2{ m_lowestPitch, sps };

    size_t i = 0; // note: same type as m_blockSize

    // Detection requires a buffer equal to two cycles of the lowest frequency
    // This introduces a slight lag - this lag aligns the realigns the detected pitch
    // with the point in the waveform where it was detected
    int minPeriod = (int)double(cycfi::q::frequency(m_lowestPitch).period() * sps);

    int num = minPeriod;
    int factor = m_regularOutputStep;

    // round to nearest boundary for m_regularOutputStep
    auto detectionOffset = (num + factor - 1 - (num - 1) % factor);
    detectionOffset = detectionOffset + m_regularOutputStep; // alignment is better when adjusted by 1 more step

    while (i < m_blockSize) {
        float s = inputBuffers[0][i];
        currentFrameWithinBlock = i;

        // Bandpass filter
        s = lp(s);
        s -= lp2(s);

        // Envelope
        auto e = env(std::abs(s));

        if (e > threshold)
        {
            // Compressor + makeup-gain + hard clip
            auto gain = float(comp(e)) * makeup_gain;
            s = clip(s * gain);
            threshold = release_threshold;
        }
        else
        {
            s = 0.0f;
            threshold = onset_threshold;
        }

        // Period Detection
        bool is_ready = m_pd->operator()(s);

        Feature f;
        f.hasTimestamp = true;

        if (is_ready) {
            m_frequency = m_pd->get_frequency();

            if (m_frequency != 0.0f) {
                pitchesWithinBlock += 1;
                if (!m_regularOutput) {
                    // output pitches as they arrive if regularOutput is off
                    timestamp = blockStartTimestamp - Vamp::RealTime::frame2RealTime(minPeriod, lrint(m_inputSampleRate)) + Vamp::RealTime::frame2RealTime(currentFrameWithinBlock, lrintf(m_inputSampleRate));

                    // guard against negative times from offset
                    if (timestamp.nsec >= 0) {
                        f.timestamp = timestamp;
                        f.values.push_back(m_frequency);
                        fs[0].push_back(f);
                    }
                }
            }
        }

        // if regularOutput is true, output values every n samples (e.g. 128)
        // - this helps to align with output of some generated datasets
        // else output frequencies with the timestamps where they occur
        if (m_regularOutput && (currentFrameWithinBlock % m_regularOutputStep) == 0) {
            // get the current sampleBlock start that we're up to, add currentFrameWithinBlock and then convert to a time
            timestamp = blockStartTimestamp - Vamp::RealTime::frame2RealTime(detectionOffset, lrint(m_inputSampleRate)) + Vamp::RealTime::frame2RealTime(currentFrameWithinBlock, lrintf(m_inputSampleRate));

            // guard against negative times from offset
            if (timestamp.nsec >= 0) {
                f.timestamp = timestamp;
                f.values.push_back(m_frequency);
                fs[0].push_back(f);
            }
        }

        ++i;
    }

    // if no pitches detected within current block output 0
    if (pitchesWithinBlock == 0) {
        m_frequency = 0.0;
    }

    return fs;
}

QlibPitch::FeatureSet
QlibPitch::getRemainingFeatures()
{
    return FeatureSet();
}

