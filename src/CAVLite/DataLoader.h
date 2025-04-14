#pragma once

#include <string>
#include "Simulation.h"
#include "Network.h"
#include "Agent.h"
#include "AgentRawInput.h"

class DataLoader
{
public:
	std::string micro_node_filepath;
	std::string micro_link_filepath;
	std::string meso_node_filepath;
	std::string meso_link_filepath;

	std::string agent_filepath;
	std::string new_agent_filepath;  // to store file path for new agents
	std::string flow_filepath;

	std::vector<AgentRawInput> all_raw_agents;


	Simulation *simulator;
	Network *net;

	void loadData(Simulation *simulator, Network *net);

	void readSettings();
	void readNetwork();
	void readSignal();
	void readAgent();
	void readFlow();

	// New Function: Reads new agents dynamically
	void LoadNewAgentsFromMemory(const AgentRawInput* raw_agents, int num_agents, int t, std::vector<Agent>& new_agents);

};