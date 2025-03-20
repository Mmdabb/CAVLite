// CAVLite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "DataLoader.h"


int main()
{
    DataLoader loader;
    Simulation simulator;
    Network net;

    loader.loadData(&simulator, &net);
    net.init();
    simulator.net = &net;
    simulator.SimulationInitialization();
    simulator.TrafficAssignment();

    // Run traffic simulation iteratively with dynamic agents
    for (int t = simulator.start_simu_interval_no; t <= simulator.end_simu_interval_no; t++)
    {
        // Load new agents at time step t
        simulator.loadVehicles(t);

        // Assign paths **only to newly added agents**
        simulator.findPathForNewAgents();

        // Run simulation step for active agents
        simulator.TrafficSimulationStep(t);
    }

    // Export final results as before
    simulator.exportSimulationResults();

    return 0;
}
