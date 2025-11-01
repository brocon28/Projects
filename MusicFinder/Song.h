// Created by bgoco on 10/28/2025.
//
#ifndef SONG_H
#define SONG_H

#include <string>
#include <cmath>
using namespace std;

enum class SortMode {
    POPULARITY,
    DANCEABILITY,
    ENERGY,
    DURATION
};

struct Song {
    string name;
    string artist;
    int popularity;
    int duration_ms;
    double danceability;
    double energy;

    int upvotes = 0;    // user/manual upvotes
    int downvotes = 0;  // user/manual downvotes
    int votes = 0;      // computed vote score shown in UI (upvotes - downvotes + base score if desired)

    bool hasUpvoted = false;
    bool hasDownvoted = false;


    static SortMode sortMode;

    Song() : popularity(0), duration_ms(0), danceability(0.0), energy(0.0),
        upvotes(0), downvotes(0), votes(0) {}

    Song(string n, string a, int p, int d = 0, double dance = 0.0, double e = 0.0)
        : name(move(n)), artist(move(a)), popularity(p),
        duration_ms(d), danceability(dance), energy(e),
        upvotes(0), downvotes(0), votes(0) {}

    bool operator>(const Song &other) const {
        switch (sortMode) {
        case SortMode::POPULARITY:   return popularity > other.popularity;
        case SortMode::DANCEABILITY: return danceability > other.danceability;
        case SortMode::ENERGY:       return energy > other.energy;
        case SortMode::DURATION:     return duration_ms > other.duration_ms;
        }
        return popularity > other.popularity;
    }

    bool operator<(const Song &other) const {
        switch (sortMode) {
        case SortMode::POPULARITY:   return popularity < other.popularity;
        case SortMode::DANCEABILITY: return danceability < other.danceability;
        case SortMode::ENERGY:       return energy < other.energy;
        case SortMode::DURATION:     return duration_ms < other.duration_ms;
        }
        return popularity < other.popularity;
    }

    // Compute the base "automatic" upvotes from features (called once after loading)
    // Note: this sets upvotes_base and downvotes_base implicitly into upvotes/downvotes if desired.
    // Here we store the computed base contribution into `votes` (but keep upvotes/downvotes zero for manual votes).
    void computeVotes() {
        double dancePoints = danceability * 100.0;
        double energyPoints = energy * 100.0;
        double durationSec = duration_ms / 1000.0;  // convert ms to seconds

        // Bell curve for duration, peak at 120s, std dev 60s
        double durationPoints = 100.0 * exp(-pow(durationSec - 120.0, 2) / (2.0 * 60.0 * 60.0));

        // Base score (not user upvotes) — scaled as you like
        double baseScore = 10.0 * (popularity + dancePoints + energyPoints + durationPoints);

        // Initialize manual vote counters to 0 (we'll keep them separate)
        upvotes = 0;
        downvotes = 1600;

        // Combined votes field to show in the UI:
        // I set it as baseScore (rounded) + manual upvotes - manual downvotes.
        votes = static_cast<int>(std::round(baseScore)) + (upvotes - downvotes);
    }

    // Manual vote operations update the `votes` field incrementally.
    void upvote() {
        ++upvotes;
        votes = votes + 1; // increment shown votes by 1 (keeps baseScore)
    }

    void downvote() {
        ++downvotes;
        votes = votes - 1; // decrement shown votes by 1
    }
};

// initialize static member
inline SortMode Song::sortMode = SortMode::POPULARITY;

#endif // SONG_H
