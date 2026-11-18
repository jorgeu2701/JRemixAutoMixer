#pragma once
#include "TrackScanner.h"
#include <random>
#include <algorithm>

struct ScheduledTrack {
    TrackItem track;
    double startBeatInAbleton = 0.0;
    double startTimeSeconds = 0.0;
};

class MixEngine {
public:
    double overlapBeats = 16.0;

    std::vector<ScheduledTrack> buildMix(std::vector<TrackItem> pool, int targetMinutes, bool shuffle) {
        std::vector<ScheduledTrack> playlist;
        if (pool.empty()) return playlist;

        if (shuffle) {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(pool.begin(), pool.end(), g);
        }

        double targetSeconds = targetMinutes * 60.0;
        double accumulatedSeconds = 0.0;
        double currentBeatCursor = 0.0;

        for (size_t i = 0; i < pool.size(); ++i) {
            if (accumulatedSeconds >= targetSeconds) break;

            ScheduledTrack scheduled;
            scheduled.track = pool[i];
            scheduled.startBeatInAbleton = currentBeatCursor;
            scheduled.startTimeSeconds = accumulatedSeconds;

            playlist.push_back(scheduled);

            double effectiveBeats = std::max(16.0, pool[i].totalBeats - overlapBeats);
            double effectiveSeconds = (effectiveBeats / pool[i].bpm) * 60.0;

            currentBeatCursor += effectiveBeats;
            accumulatedSeconds += effectiveSeconds;
        }
        return playlist;
    }
};
