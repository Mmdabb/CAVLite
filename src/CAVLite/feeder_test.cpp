#include "AgentInputFeeder.h"
#include <iostream>

int main() {
    std::string csv_file = "test_data/agents.csv"; 
    double simulation_step = 5.0;

    AgentInputFeeder feeder(csv_file, simulation_step);
    feeder.loadAllAgentsFromCSV();

    const auto& agents = feeder.getAllAgents();
    for (const auto& agent : agents) {
        std::cout << "Agent ID: " << agent.agent_id
            << ", O: " << agent.o_node_id
            << ", D: " << agent.d_node_id
            << ", Vol: " << agent.volume
            << ", DepTime: " << agent.departure_time
            << ", NodeSeq: " << (agent.node_sequence ? agent.node_sequence : "null")
            << std::endl;
    }

    return 0;
}
