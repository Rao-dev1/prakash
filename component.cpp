#include "component.h"

Component::Component() 
    : componentID(""), lotID(""),
      leakageDrift(0.0), iddqDrift(0.0),
      leakagePosZ(0.0), iddqPosZ(0.0), positionScore(0.0),
      leakageDriftZ(0.0), iddqDriftZ(0.0), driftScore(0.0),
      forecastRisk(0.0), riskScore(0.0),
      limitViolated(false), abnormalIndicatorsCount(0),
      decision("PENDING"), reason("")
{
    leakage[0] = leakage[1] = leakage[2] = 0.0;
    iddq[0] = iddq[1] = iddq[2] = 0.0;
}

Component::Component(const std::string& id, const std::string& lot, 
                     double l0, double l24, double i0, double i24)
    : componentID(id), lotID(lot),
      leakagePosZ(0.0), iddqPosZ(0.0), positionScore(0.0),
      leakageDriftZ(0.0), iddqDriftZ(0.0), driftScore(0.0),
      forecastRisk(0.0), riskScore(0.0),
      limitViolated(false), abnormalIndicatorsCount(0),
      decision("PENDING"), reason("")
{
    leakage[0] = l0;
    leakage[1] = l24;
    // Section 13: Linear 168h extrapolation formula:
    // Rate = (l24 - l0) / 24, l168 = l24 + Rate * 144 = 7 * l24 - 6 * l0
    leakage[2] = 7.0 * l24 - 6.0 * l0;

    iddq[0] = i0;
    iddq[1] = i24;
    // Linear 168h extrapolation for iddq:
    iddq[2] = 7.0 * i24 - 6.0 * i0;

    // Early drift (24h - 0h)
    leakageDrift = l24 - l0;
    iddqDrift = i24 - i0;
}
