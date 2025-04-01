#pragma once
#include <vector>
#include <string>
#include "AgentRawInput.h"

class AgentInputFeeder {
public:
    AgentInputFeeder(const std::string& csv_path, double sim_step);
    void loadAllAgentsFromCSV();
    const std::vector<AgentRawInput>& getAllAgents() const;

private:
    std::string file_path;
    double simulation_step;
    std::vector<AgentRawInput> all_agents;
};
