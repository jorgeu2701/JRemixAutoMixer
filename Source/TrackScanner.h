#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>

struct TrackItem {
    juce::File audioFile;
    juce::String title;
    double durationSeconds = 0.0;
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

        std::set<juce::String> seenSongKeys;

        for (const auto& file : files) {
            TrackItem item;
            item.audioFile = file;
            item.title = file.getFileNameWithoutExtension();

            juce::String uniqueKey = buildSongArtistKey(item.title);
            if (seenSongKeys.count(uniqueKey) > 0) {
                continue;
            }

            if (auto* reader = formatManager.createReaderFor(file)) {
                if (reader->sampleRate > 0.0 && reader->lengthInSamples > 0) {
                    item.durationSeconds = static_cast<double>(reader->lengthInSamples) / reader->sampleRate;
                }
                delete reader;
            }

            // Solo aceptar audios validos entre 30 segundos y 15 minutos
            if (item.durationSeconds < 30.0 || item.durationSeconds > 900.0) {
                continue;
            }

            seenSongKeys.insert(uniqueKey);
            results.push_back(item);
        }
        return results;
    }

private:
    static juce::String buildSongArtistKey(const juce::String& rawTitle) {
        juce::StringArray parts;
        parts.addTokens(rawTitle, "-", "\"");

        juce::String part1 = parts.size() > 0 ? cleanToken(parts[0]) : "";
        juce::String part2 = parts.size() > 1 ? cleanToken(parts[1]) : "";

        if (part2.isNotEmpty()) {
            return (part1 < part2) ? (part1 + "|" + part2) : (part2 + "|" + part1);
        }
        return part1;
    }

    static juce::String cleanToken(juce::String text) {
        while (text.contains("(") && text.contains(")")) {
            int start = text.indexOf("(");
            int end = text.indexOf(start, ")");
            if (end > start) text = text.substring(0, start) + text.substring(end + 1);
            else break;
        }
        while (text.contains("[") && text.contains("]")) {
            int start = text.indexOf("[");
            int end = text.indexOf(start, "]");
            if (end > start) text = text.substring(0, start) + text.substring(end + 1);
            else break;
        }

        text = text.toLowerCase().trim();
        text = text.removeCharacters(".,'\"!?_");
        while (text.contains("  ")) text = text.replace("  ", " ");
        return text;
    }
};
