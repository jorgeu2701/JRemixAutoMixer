#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>

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

        std::set<juce::String> seenSongKeys;

        for (const auto& file : files) {
            TrackItem item;
            item.audioFile = file;
            item.title = file.getFileNameWithoutExtension();

            // Generar clave unica de Cancion + Artista para evitar versiones repetidas
            juce::String uniqueKey = buildSongArtistKey(item.title);
            if (seenSongKeys.count(uniqueKey) > 0) {
                continue; // Omitir si ya existe otra version de la misma cancion y artista
            }

            if (auto* reader = formatManager.createReaderFor(file)) {
                item.durationSeconds = reader->lengthInSamples / reader->sampleRate;
                delete reader;
            }

            if (item.durationSeconds <= 5.0) continue;

            juce::File asdFile(file.getFullPathName() + ".asd");
            if (asdFile.existsAsFile()) {
                item.hasAbletonCache = true;
                item.bpm = extractBpmFromAsd(asdFile, item.title);
            } else {
                item.bpm = extractBpmFromFilename(item.title);
            }

            item.totalBeats = (item.durationSeconds / 60.0) * item.bpm;
            seenSongKeys.insert(uniqueKey);
            results.push_back(item);
        }
        return results;
    }

private:
    static juce::String buildSongArtistKey(const juce::String& rawTitle) {
        // Dividir por " - " (formato DJ Pool: "Cancion - Artista - Remixer - Genero...")
        juce::StringArray parts;
        parts.addTokens(rawTitle, "-", "\"");

        juce::String part1 = parts.size() > 0 ? cleanToken(parts[0]) : "";
        juce::String part2 = parts.size() > 1 ? cleanToken(parts[1]) : "";

        // Ordenar ambas partes alfabeticamente para detectar tambien si estan invertidas ("Artista - Cancion" vs "Cancion - Artista")
        if (part2.isNotEmpty()) {
            return (part1 < part2) ? (part1 + "|" + part2) : (part2 + "|" + part1);
        }
        return part1;
    }

    static juce::String cleanToken(juce::String text) {
        // Quitar texto entre parentesis o corchetes como (Clean), (Dirty), (Extended), [Intro]
        while (text.contains("(") && text.contains(")")) {
            int start = text.indexOf("(");
            int end = text.indexOf(start, ")");
            if (end > start)
                text = text.substring(0, start) + text.substring(end + 1);
            else
                break;
        }
        while (text.contains("[") && text.contains("]")) {
            int start = text.indexOf("[");
            int end = text.indexOf(start, "]");
            if (end > start)
                text = text.substring(0, start) + text.substring(end + 1);
            else
                break;
        }

        // Normalizar a minusculas, quitar espacios extra y signos de puntuacion
        text = text.toLowerCase().trim();
        text = text.removeCharacters(".,'\"!?_");
        while (text.contains("  "))
            text = text.replace("  ", " ");
        return text;
    }

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
