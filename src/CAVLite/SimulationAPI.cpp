#include "SimulationAPI.h"
#include "CACF.h"
#include <iostream>

SimulationAPI::SimulationAPI() {}

//SimulationAPI::~SimulationAPI() {
//    if (feeder) delete feeder;
//}

bool SimulationAPI::initialize(int& start_interval, int& end_interval, int& simu_step) {
    //simulator.simulation_start_time = sim_start;
    //simulator.simulation_end_time = sim_end;
    //simulator.simulation_step = step;
    //simulator.cflc_model = model;
    //simulator.total_assignment_iteration = 10;

    loader.loadData(&simulator, &net);
    net.init();

    simulator.net = &net;
    simulator.SimulationInitialization();
    simulator.TrafficAssignment();
    simulator.exportInitialAssignment("initial_assignment.csv");
    simulator.exportLinkPerformance("initial_link_perf.csv");

    //feeder = new AgentInputFeeder(agent_file, step);
    //feeder->loadAllAgentsFromCSV();
    //loader.all_raw_agents = feeder->getAllAgents();

    if (simulator.cflc_model == "CACF")
        VehControllerCA::init(&net, simulator.simulation_step);

    start_interval = simulator.start_simu_interval_no;
    end_interval = simulator.end_simu_interval_no;
    simu_step = simulator.simulation_step;

    return true;
}

void SimulationAPI::load_and_run_step(int t, const std::vector<AgentRawInput>& external_raw_agents) {
    
    std::vector<Agent> new_agents;
    loader.LoadNewAgentsFromMemory(external_raw_agents.data(), static_cast<int>(external_raw_agents.size()), t, new_agents);
    simulator.loadVehicles(t, new_agents);
    simulator.findPathForNewAgents();
    simulator.TrafficSimulationStep(t);
}

void SimulationAPI::finalize() {
    simulator.exportSimulationResults();
}

// C API for DLL
SimulationAPI* create_simulation() {
    return new SimulationAPI();
}
void destroy_simulation(SimulationAPI* sim) {
    delete sim;
}


extern "C" {

    DLL_PUBLIC bool simulation_initialize(SimulationAPI* sim, int* start, int* end, int* step) {
        return sim->initialize(*start, *end, *step);
    }

    DLL_PUBLIC void simulation_load_and_run_step(SimulationAPI* sim, int t, AgentRawInput* agents, int count) {
        std::vector<AgentRawInput> agent_vec(agents, agents + count);
        sim->load_and_run_step(t, agent_vec);
    }

    DLL_PUBLIC void simulation_finalize(SimulationAPI* sim) {
        sim->finalize();
    }

}
