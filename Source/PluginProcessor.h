#pragma once
#include <JuceHeader.h>
#include "MixEngine.h"

class JRemixAudioProcessor : public juce::AudioProcessor {
public:
    JRemixAudioProcessor();
    ~JRemixAudioProcessor() override = default;

    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    std::vector<TrackItem> loadedPool;
    std::vector<ScheduledTrack> currentPlaylist;
    MixEngine mixEngine;
    int targetMinutes = 45;
    juce::String currentFolderName = "Arrastra tu carpeta de musica aqui";

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JRemixAudioProcessor)
};
