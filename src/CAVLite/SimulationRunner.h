#pragma once

#ifdef _WIN32
#ifdef SIMULATIONRUNNER_EXPORTS
#define SIM_API __declspec(dllexport)
#else
#define SIM_API __declspec(dllimport)
#endif
#else
#define SIM_API
#endif

extern "C" {
    /**
     * Runs the full simulation as defined in the original main().
     * Parameters:
     * - agent_csv_path: Path to the CSV file containing dynamic agents.
     * - output_prefix: Prefix for result files (e.g., "output" will create "output_agents.csv").
     * Returns:
     * - 0 on success, non-zero on failure.
     */
    SIM_API int run_full_simulation(const char* agent_csv_path, const char* output_prefix);
}
