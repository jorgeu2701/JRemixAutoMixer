#pragma once
#include "MixEngine.h"

class BridgeExporter {
public:
    static bool sendToAbleton(const std::vector<ScheduledTrack>& playlist) {
        juce::File tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getChildFile("JRemix_VST3_Bridge");
        if (!tempDir.exists()) tempDir.createDirectory();

        juce::File orderFile = tempDir.getChildFile("jremix_order.json");
        juce::File triggerFile = tempDir.getChildFile("trigger.flag");

        juce::DynamicObject::Ptr rootObj = new juce::DynamicObject();
        juce::Array<juce::var> tracksArray;

        for (size_t i = 0; i < playlist.size(); ++i) {
            juce::DynamicObject::Ptr tObj = new juce::DynamicObject();
            juce::String trackLabel = juce::String::formatted("%02d - ", (int)i + 1) + playlist[i].track.title;
            
            tObj->setProperty("name", trackLabel);
            tObj->setProperty("path", playlist[i].track.audioFile.getFullPathName());
            tObj->setProperty("start_beat", playlist[i].startBeatInAbleton);
            tracksArray.add(juce::var(tObj.get()));
        }

        rootObj->setProperty("tracks", tracksArray);
        juce::String jsonString = juce::JSON::toString(juce::var(rootObj.get()));

        if (orderFile.replaceWithText(jsonString)) {
            triggerFile.create();
            return true;
        }
        return false;
    }

    static bool exportTracklistTxt(const std::vector<ScheduledTrack>& playlist, const juce::File& targetFile) {
        juce::String output = "JREMIX AUTO-MIXER - TRACKLIST\n====================================\n\n";
        for (size_t i = 0; i < playlist.size(); ++i) {
            int totalSecs = static_cast<int>(playlist[i].startTimeSeconds);
            output += juce::String::formatted("[%02d:%02d] %02d. %s (%.1f BPM)\n",
                                              totalSecs / 60, totalSecs % 60, (int)i + 1,
                                              playlist[i].track.title.toRawUTF8(),
                                              playlist[i].track.bpm);
        }
        return targetFile.replaceWithText(output);
    }
};
