#include <iostream>
#include "AgentRawInput.h"
#include "SimulationAPI.h"
#include "AgentInputFeeder.h"


int main() {
    int start_interval, end_interval, simulation_step;
    SimulationAPI* sim = create_simulation();

    sim->initialize(start_interval, end_interval, simulation_step);

    //AgentInputFeeder feeder("full_agent_file.csv", simulation_step);
    AgentInputFeeder feeder("C:\\Users\\mabbas10\\source\\repos\\CAVLite\\dataset\\3-corridor\\full_agent_file.csv", simulation_step);

    feeder.loadAllAgentsFromCSV();
    std::vector<AgentRawInput> all_raw_agents = feeder.getAllAgents();


    for (int t = start_interval; t < end_interval; ++t) {
        std::vector<AgentRawInput> filtered;
        double lower = t * simulation_step / 60.0;
        double upper = (t + 1) * simulation_step / 60.0;

        for (const auto& agent : all_raw_agents) {
            if (agent.departure_time >= lower && agent.departure_time < upper)
                filtered.push_back(agent);
        }

        sim->load_and_run_step(t, filtered);
    }

    sim->finalize();
    destroy_simulation(sim);

    std::cout << "Simulation complete.\n";
    return 0;
}