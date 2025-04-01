#pragma once

struct AgentRawInput {
    int agent_id;
    int o_node_id;
    int d_node_id;
    float departure_time;
    float volume;
    const char* node_sequence;  // or std::string if not exporting as DLL
};
