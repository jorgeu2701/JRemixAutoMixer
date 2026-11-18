#include "PluginProcessor.h"
#include "PluginEditor.h"

JRemixAudioProcessor::JRemixAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)) {}

juce::AudioProcessorEditor* JRemixAudioProcessor::createEditor() {
    return new JRemixAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new JRemixAudioProcessor();
}
