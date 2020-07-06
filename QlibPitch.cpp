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
        m_threshold(-45.0)
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
    lowestPitch.description = "Lower limit for pitch tracking output. Must be within 4 octaves of highest pitch. Defaults to 100Hz";
    lowestPitch.unit = "Hz";
    lowestPitch.minValue = 20;
    lowestPitch.maxValue = 2000;
    lowestPitch.defaultValue = 100.0;
    lowestPitch.isQuantized = false;
    list.push_back(lowestPitch);

    ParameterDescriptor highestPitch;
    highestPitch.identifier = "highestPitch";
    highestPitch.name = "Highest Pitch";
    highestPitch.description = "Upper limit for pitch tracking output. Must be within 4 octaves of lowest pitch. Defaults to 800Hz";
    highestPitch.unit = "Hz";
    highestPitch.minValue = 20;
    highestPitch.maxValue = 2000;
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

    float frequency = 0.0f;

    size_t i = 0; // note: same type as m_blockSize

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

        if (is_ready)
        {
            frequency = m_pd->get_frequency();

            if (frequency != 0.0f)
            {
                pitchesWithinBlock += 1;
                // plan here - get the current sample that we're up to, add i and then convert to a time
                timestamp = blockStartTimestamp + Vamp::RealTime::frame2RealTime(currentFrameWithinBlock, lrintf(m_inputSampleRate));

                f.timestamp = timestamp;
                f.values.push_back(frequency);
                fs[0].push_back(f);
            }
        }

        ++i;
    }

    // if no pitches detected within current block output 0
    if (pitchesWithinBlock == 0) {
        f.timestamp = timestamp;
        f.values.push_back(0.0f);
        fs[0].push_back(f);
    }

    return fs;
}

QlibPitch::FeatureSet
QlibPitch::getRemainingFeatures()
{
    return FeatureSet();
}

