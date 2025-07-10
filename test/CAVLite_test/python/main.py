# main.py
from simu_wrapper import Simu, AgentRawInput
import csv
import os

# Set working directory to where your dataset is
os.chdir(r"C:\Users\mabbas10\source\repos\CAVLite\dataset\3-corridor")

def load_agents_from_csv(path):
    agents = []
    with open(path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            try:
                agent_id = int(row["agent_id"])
                o_node_id = int(row["o_node_id"])
                d_node_id = int(row["d_node_id"])
                o_zone_id = int(row["o_zone_id"])
                d_zone_id = int(row["d_zone_id"])
                volume = float(row["volume"])
                departure_time = float(row["departure_time"])
                node_sequence_str = row.get("node_sequence", "")
                node_sequence = node_sequence_str.encode() if node_sequence_str else None

                agents.append(AgentRawInput(
                    agent_id=agent_id,
                    o_node_id=o_node_id,
                    d_node_id=d_node_id,
                    o_zone_id=o_zone_id,
                    d_zone_id=d_zone_id,
                    volume=volume,
                    departure_time=departure_time,
                    node_sequence=node_sequence
                ))
            except Exception as e:
                print(f"Skipping invalid row: {row}\nError: {e}")
    print(f"Loaded {len(agents)} agents from CSV.")
    return agents


def main():
    sim = Simu()
    step_sec = sim.step.value
    start = sim.start.value
    end = sim.end.value

    # Load all agents once from CSV
    csv_path = r"C:\Users\mabbas10\source\repos\CAVLite\dataset\3-corridor\full_agent_file.csv"
    all_agents = load_agents_from_csv(csv_path)

    # Run simulation step by step
    for t in range(start, end):
        lower = t * step_sec / 60.0
        upper = (t + 1) * step_sec / 60.0
        agents_this_step = [
            a for a in all_agents if lower <= a.departure_time < upper
        ]
        sim.run_step(t, agents_this_step)

    sim.finalize()
    print("Simulation complete.")

if __name__ == "__main__":
    main()
