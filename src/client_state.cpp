#include "client_state.h"

using namespace std;
/*
    client_state.cpp

    Purpose:
    Implements per-client state tracking.

    Current behavior:
    - NORMAL events increase normal count.
    - SUSPICIOUS events increase suspicious count.
    - CRITICAL events increase critical count.
    - 3 or more suspicious events escalates the client to CRITICAL.
    - Any critical event immediately marks the client as CRITICAL.

    This gives the project memory:
    the server does not just classify one message;
    it tracks behavior over time.
*/

ClientStateUpdate ClientStateTracker::updateClientState(
    const string &client_id,
    const ThreatResult &threat)
{
    /*
    Lock the mutex because multiple client states may access
    */
    lock_guard<mutex> lock(state_mutex);

    /*
    Access or create stats for this client
    if client_id does not exist, unordered_map creates a default
    ClientStats obj

    */

    ClientStats &stats = clients[client_id];
    stats.total_envents++;
    string update_reason;

    // update counters based on the latext threat level
    if (threat.level == ThreatLevel::NORMAL)
    {
        stats.normal_events++;

        /*
            Normal events do not automatically reset a suspicious
            or critical client in this first version.

            This keeps state conservative:
            once a client becomes suspicious/critical, future logic
            should decide when it is safe to downgrade.
        */

        if (stats.current_state == ClientThreatState::NORMAL)
        {
            update_reason = "Client remains normal";
        }
        else
        {
            update_reason = "Normal events received, client state unchange";
        }
    }
    else if (threat.level == ThreatLevel::SUSPICIOUS)
    {
        stats.suspicious_events++;

        // Escalate repeated suspicious activity
        if (stats.suspicious_events >= 3)
        {
            stats.current_state = ClientThreatState::CRITICAL;
            update_reason = "Repeated suspicious activity escalated to critical";
        }
        else
        {
            stats.current_state = ClientThreatState::SUSPICIOUS;
            update_reason = "Client marked suspicious";
        }
    }
    else if (threat.level == ThreatLevel::CRITICAL)
    {
        stats.critical_events++;
        stats.current_state = ClientThreatState::CRITICAL;
        update_reason = "Critical activity detected";
    }

    // Build the update object return to the caller
    ClientStateUpdate update;
    update.client_id = client_id;
    update.current_state = stats.current_state;
    update.total_events = stats.total_envents;
    update.normal_events = stats.normal_events;
    update.suspicious_events = stats.suspicious_events;
    update.critical_events = stats.critical_events;
    update.reason = update_reason;

    return update;
}

string ClientStateTracker::stateToString(ClientThreatState state)
{
    switch (state)
    {
    case ClientThreatState::NORMAL:
        return "NORMAL";
    case ClientThreatState::SUSPICIOUS:
        return "SUSPICIOUS";
    case ClientThreatState::CRITICAL:
        return "CRITICAL";

    default:
        return "UNKNOWN";
    }
}
