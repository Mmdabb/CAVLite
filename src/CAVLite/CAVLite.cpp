// CAVLite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "DataLoader.h"
#include "CACF.h"


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

    if (simulator.cflc_model == "CACF")
        VehControllerCA::init(&net, simulator.simulation_step);

    int cumulative_count = 0;
    int print_frequency = round(120.0 / simulator.simulation_step);

    // Run traffic simulation iteratively with dynamic agents
    for (int t = simulator.start_simu_interval_no; t <= simulator.end_simu_interval_no; t++)
    {
        // Load new agents at time step t
        simulator.loadVehicles(t);

        // Assign paths only to newly added agents
        simulator.findPathForNewAgents();

        // Run simulation step for active agents
        simulator.TrafficSimulationStep(t);

        if (cumulative_count % print_frequency == 0)
        {
            float simulation_min = t * simulator.simulation_step / 60.0;
            printf("\rTraffic Simulation...%.1f|%.1f (current time | end time)",
                simulation_min, simulator.simulation_end_time);
        }
        cumulative_count++;

    }

    // Export final results
    // simulator.exportSimulationResults();

    std::cout << " Done\n";

    return 0;
}
