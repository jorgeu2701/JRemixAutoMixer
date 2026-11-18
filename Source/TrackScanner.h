#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include <cstring>

struct TrackItem {
    juce::File audioFile;
    juce::String title;
    double durationSeconds = 0.0;
    double bpm = 128.0;
    double totalBeats = 0.0;
    bool hasAbletonCache = false;
};

class TrackScanner {
public:
    static std::vector<TrackItem> scanFolder(const juce::File& folder) {
        std::vector<TrackItem> results;
        if (!folder.isDirectory()) return results;

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        juce::Array<juce::File> files = folder.findChildFiles(
            juce::File::findFiles, false, "*.mp3;*.wav;*.aiff;*.flac;*.m4a"
        );

        for (const auto& file : files) {
            TrackItem item;
            item.audioFile = file;
            item.title = file.getFileNameWithoutExtension();

            if (auto* reader = formatManager.createReaderFor(file)) {
                item.durationSeconds = reader->lengthInSamples / reader->sampleRate;
                delete reader;
            }

            juce::File asdFile(file.getFullPathName() + ".asd");
            if (asdFile.existsAsFile()) {
                item.hasAbletonCache = true;
                item.bpm = extractBpmFromAsd(asdFile, item.title);
            } else {
                item.bpm = extractBpmFromFilename(item.title);
            }

            item.totalBeats = (item.durationSeconds / 60.0) * item.bpm;
            if (item.durationSeconds > 5.0) {
                results.push_back(item);
            }
        }
        return results;
    }

private:
    static double extractBpmFromAsd(const juce::File& asdFile, const juce::String& fallbackName) {
        double bpmFromName = extractBpmFromFilename(fallbackName);
        if (bpmFromName > 0.0) return bpmFromName;

        juce::MemoryBlock data;
        if (asdFile.loadFileAsData(data) && data.getSize() > 64) {
            const uint8_t* bytes = static_cast<const uint8_t*>(data.getData());
            for (size_t i = 0; i < data.getSize() - 8; ++i) {
                double val;
                std::memcpy(&val, bytes + i, sizeof(double));
                if (val >= 70.0 && val <= 175.0 && std::floor(val * 100) == val * 100) {
                    return val;
                }
            }
        }
        return 128.0;
    }

    static double extractBpmFromFilename(const juce::String& name) {
        juce::StringArray tokens;
        tokens.addTokens(name, " -_[]()", "");
        for (int i = 0; i < tokens.size(); ++i) {
            double val = tokens[i].getDoubleValue();
            if (val >= 70.0 && val <= 175.0) return val;
        }
        return 0.0;
    }
};
