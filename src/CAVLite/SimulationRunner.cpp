#include "SimulationRunner.h"
#include "DataLoader.h"
#include "CACF.h"
#include "AgentInputFeeder.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

std::ofstream debug_log_file("backward_tree_debug.txt");

int run_full_simulation(const char* agent_csv_path, const char* output_prefix)
{
    try {
        DataLoader loader;
        Simulation simulator;
        Network net;

        // Step 1: Load network and simulation settings
        loader.loadData(&simulator, &net);
        net.init();
        simulator.net = &net;
        simulator.SimulationInitialization();
        simulator.TrafficAssignment();

        // Step 2: Export initial assignment and link performance
        simulator.exportInitialAssignment(std::string(output_prefix) + "_initial_assignment.csv");
        simulator.exportLinkPerformance(std::string(output_prefix) + "_initial_link_performance.csv");

        // Step 3: Load agents from external CSV
        AgentInputFeeder feeder(agent_csv_path, simulator.simulation_step);
        feeder.loadAllAgentsFromCSV();
        loader.all_raw_agents = feeder.getAllAgents();

        // Step 4: Initialize car-following model if needed
        if (simulator.cflc_model == "CACF")
            VehControllerCA::init(&net, simulator.simulation_step);

        // Step 5: Run simulation over time steps
        int cumulative_count = 0;
        int print_frequency = std::round(120.0 / simulator.simulation_step);

        for (int t = simulator.start_simu_interval_no; t <= simulator.end_simu_interval_no; ++t)
        {
            // Filter dynamic agents for time t
            std::vector<AgentRawInput> filtered_inputs;
            double lower = t * simulator.simulation_step / 60;
            double upper = (t + 1) * simulator.simulation_step / 60;

            for (const auto& raw : loader.all_raw_agents) {
                if (raw.departure_time >= lower && raw.departure_time < upper)
                    filtered_inputs.push_back(raw);
            }

            std::vector<Agent> new_agents;
            loader.LoadNewAgentsFromMemory(filtered_inputs.data(), static_cast<int>(filtered_inputs.size()), t, new_agents);
            simulator.loadVehicles(t, new_agents);
            simulator.findPathForNewAgents();
            simulator.TrafficSimulationStep(t);

            if (cumulative_count % print_frequency == 0) {
                float simulation_min = t * simulator.simulation_step / 60.0f;
                std::cout << "\rTraffic Simulation... " << simulation_min << " | " << simulator.simulation_end_time << " (current | end time).\n";
            }
            cumulative_count++;
        }

        // Step 6: Final export
        simulator.output_filename = std::string(output_prefix) + "_agents.csv";
        simulator.exportSimulationResults();

        debug_log_file.close();
        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "[ERROR] Simulation failed: " << ex.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "[ERROR] Unknown error occurred during simulation.\n";
        return 2;
    }
}
