#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>

// Core struct grouping all burn-in measurements and analytics for one electronic component
struct Component {
    std::string componentID;
    std::string lotID;

    // Time-series array measurements (0h, 24h, 168h forecast)
    // Index 0: 0h
    // Index 1: 24h
    // Index 2: 168h (linear forecast)
    double leakage[3];
    double iddq[3];

    // Degradation (24h - 0h)
    double leakageDrift;
    double iddqDrift;

    // Statistical Deviation Scores (relative to lot population)
    double leakagePosZ;
    double iddqPosZ;
    double positionScore; // max(leakagePosZ, iddqPosZ)

    double leakageDriftZ;
    double iddqDriftZ;
    double driftScore;    // max(leakageDriftZ, iddqDriftZ)

    double forecastRisk;  // Risk indicator derived from 168h projection
    double riskScore;     // Combined latent defect risk score for Max Heap prioritization

    // Engineering limits check
    bool limitViolated;

    // Abnormal indicators count
    int abnormalIndicatorsCount;

    // Screening Decision: PASS, MONITOR, SUSPECT, REJECT
    std::string decision;
    std::string reason;

    Component();
    Component(const std::string& id, const std::string& lot, 
              double l0, double l24, double i0, double i24);
};

#endif // COMPONENT_H
