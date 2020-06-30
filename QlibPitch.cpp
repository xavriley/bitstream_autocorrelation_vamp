#include "QlibPitch.h"

namespace q = cycfi::q;
using namespace q::literals;
using std::fixed;

QlibPitch::QlibPitch(float inputSampleRate) :
    Plugin(inputSampleRate),
        m_lowestPitch(100.0),
        m_highestPitch(800.0),
        m_blockSize(0)
    // Also be sure to set your plugin parameters (presumably stored
    // in member variables) to their default values here -- the host
    // will not do that for you
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

    return list;
}

float
QlibPitch::getParameter(string identifier) const
{
    if (identifier == "lowestPitch") {
        return static_cast<float>(m_lowestPitch);
    } else if (identifier == "highestPitch") {
        return static_cast<float>(m_highestPitch);
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
    d.sampleType = OutputDescriptor::OneSamplePerStep;
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
    Feature f;
    f.hasTimestamp = true;

    q::pitch_detector    pd(m_lowestPitch, m_highestPitch, m_inputSampleRate, -45_dB);
    //q::pitch_detector    pd{400_Hz, 450_Hz, (std::uint32_t)m_inputSampleRate, -45_dB};

    float frequency = 0.0f;

    size_t i = 0; // note: same type as m_blockSize

    while (i < m_blockSize) {
        float sample = inputBuffers[0][i];

        // Period Detection
        bool is_ready = pd(sample);

        if (is_ready)
        {
            frequency = pd.get_frequency();

            if (frequency != 0.0f)
            {
                f.values.push_back(frequency);
            }
        }

        ++i;
    }

    FeatureSet fs;
    fs[0].push_back(f);
    return fs;
}

QlibPitch::FeatureSet
QlibPitch::getRemainingFeatures()
{
    return FeatureSet();
}

