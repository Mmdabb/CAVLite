#pragma once

#include "DataLoader.h"
#include "Simulation.h"
#include "Network.h"

#ifdef _WIN32
#ifdef EXPORTING_DLL
#define DLL_PUBLIC __declspec(dllexport)
#else
#define DLL_PUBLIC __declspec(dllimport)
#endif
#else
#define DLL_PUBLIC
#endif

class DLL_PUBLIC SimulationAPI {
public:
    SimulationAPI();
    //~SimulationAPI();

    bool initialize(int& start_interval, int& end_interval, int& simu_step);
    //void load_and_run_step(int t);
    void load_and_run_step(int t, const std::vector<AgentRawInput>& external_raw_agents);
    void finalize();

private:
    DataLoader loader;
    Simulation simulator;
    Network net;
};

extern "C" {
    DLL_PUBLIC SimulationAPI* create_simulation();
    DLL_PUBLIC void destroy_simulation(SimulationAPI* sim);
}
