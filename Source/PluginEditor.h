#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BridgeExporter.h"

class JRemixAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::FileDragAndDropTarget {
public:
    JRemixAudioProcessorEditor (JRemixAudioProcessor&);
    ~JRemixAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

private:
    void rebuildPlaylist(bool shuffle);

    JRemixAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label dropZoneLabel;
    juce::Label minutesLabel;
    juce::Slider minutesSlider;
    juce::TextButton randomButton { "RANDOM" };
    juce::TextButton generateButton { "GENERATE MIX" };
    juce::TextButton exportButton { "EXPORT TRACKLIST" };
    juce::TextEditor tracklistDisplay;
    juce::Label statusLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JRemixAudioProcessorEditor)
};
