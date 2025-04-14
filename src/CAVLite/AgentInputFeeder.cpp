#include "AgentInputFeeder.h"
#include <fstream>
#include <sstream>
#include <cstring>  // for strdup
#include "CSVParser.h" 



AgentInputFeeder::AgentInputFeeder(const std::string& csv_path, double sim_step)
    : file_path(csv_path), simulation_step(sim_step) {
}

void AgentInputFeeder::loadAllAgentsFromCSV() {
    CCSVParser parser;
    //parser.Delimiter = '\t';
    if (!parser.OpenCSVFile(file_path, true)) {
        std::cerr << "Failed to open CSV: " << file_path << std::endl;
        return;
    }

    int agent_id, o_node_id, d_node_id, o_zone_id, d_zone_id;
    float volume, departure_time;
    std::string node_sequence;

    //for (const auto& name : parser.GetHeaderList())
    //    std::cout << "[" << name << "]" << std::endl;

    //int line_number = 0;

    while (parser.ReadRecord()) {
        //++line_number;
        //std::cout << "Reading row " << line_number << ": ";
        //for (auto& val : parser.GetLineRecord()) std::cout << val << " | ";
        //std::cout << "\n";

        AgentRawInput input;

        if (!parser.GetValueByFieldName("agent_id", agent_id)) continue;

        //if (parser.GetValueByFieldName("agent_id", agent_id) == false) {
        //    std::cout << "Missing agent_id in row " << line_number << std::endl;
        //    continue;
        //}

        if (!parser.GetValueByFieldName("o_zone_id", o_zone_id)) continue;
        if (!parser.GetValueByFieldName("d_zone_id", d_zone_id)) continue;
        if (!parser.GetValueByFieldName("o_node_id", o_node_id)) continue;
        if (!parser.GetValueByFieldName("d_node_id", d_node_id)) continue;
        if (!parser.GetValueByFieldName("volume", volume)) continue;
        if (!parser.GetValueByFieldName("departure_time", departure_time)) continue;

        parser.GetValueByFieldName("node_sequence", node_sequence);  // optional

        input.agent_id = agent_id;
        input.o_node_id = o_node_id;
        input.d_node_id = d_node_id;
        input.volume = volume;
        input.departure_time = departure_time;
        input.node_sequence = node_sequence;

        all_agents.push_back(input);
    }

    std::cout << "Loaded " << all_agents.size() << " agents\n";
}

const std::vector<AgentRawInput>& AgentInputFeeder::getAllAgents() const {
    return all_agents;
}
