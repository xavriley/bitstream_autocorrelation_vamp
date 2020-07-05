#ifndef QLIB_PITCH_H
#define QLIB_PITCH_H

#include <vamp-sdk/Plugin.h>
#include <q/support/literals.hpp>
#include <q/pitch/pitch_detector.hpp>
#include <q/fx/envelope.hpp>
#include <q/fx/lowpass.hpp>
#include <q/fx/biquad.hpp>
#include <q/fx/dynamic.hpp>
#include <q/fx/waveshaper.hpp>

using std::string;


class QlibPitch : public Vamp::Plugin
{
public:
    QlibPitch(float inputSampleRate);
    virtual ~QlibPitch();

    string getIdentifier() const;
    string getName() const;
    string getDescription() const;
    string getMaker() const;
    int getPluginVersion() const;
    string getCopyright() const;

    InputDomain getInputDomain() const;
    size_t getPreferredBlockSize() const;
    size_t getPreferredStepSize() const;
    size_t getMinChannelCount() const;
    size_t getMaxChannelCount() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(string identifier) const;
    void setParameter(string identifier, float value);

    ProgramList getPrograms() const;
    string getCurrentProgram() const;
    void selectProgram(string name);

    OutputList getOutputDescriptors() const;

    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();
    std::shared_ptr<cycfi::q::pitch_detector> m_pd;

protected:
    // plugin-specific data and methods go here
    size_t m_blockSize;
    float m_highestPitch;
    float m_lowestPitch;
    float m_threshold;
};



#endif
