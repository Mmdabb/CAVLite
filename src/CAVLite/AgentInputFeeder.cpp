#include "AgentInputFeeder.h"
#include <fstream>
#include <sstream>
#include <iostream>

AgentInputFeeder::AgentInputFeeder(const std::string& csv_path, double sim_step)
    : file_path(csv_path), simulation_step(sim_step) {
}

void AgentInputFeeder::loadAllAgentsFromCSV() {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open CSV: " << file_path << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> tokens;

        while (std::getline(ss, cell, '\t')) tokens.push_back(cell);
        if (tokens.size() < 19) continue;

        AgentRawInput input;
        try {
            input.agent_id = std::stoi(tokens[1]);
            input.o_node_id = std::stoi(tokens[2]);
            input.d_node_id = std::stoi(tokens[3]);
            input.volume = std::stof(tokens[17]);
            input.departure_time = std::stof(tokens[18]);
            input.node_sequence = tokens[6].empty() ? nullptr : strdup(tokens[6].c_str());
            all_agents.push_back(input);
        }
        catch (...) {
            std::cerr << "Skipping bad row\n";
        }
    }

    std::cout << "Loaded " << all_agents.size() << " agents\n";
}

const std::vector<AgentRawInput>& AgentInputFeeder::getAllAgents() const {
    return all_agents;
}
