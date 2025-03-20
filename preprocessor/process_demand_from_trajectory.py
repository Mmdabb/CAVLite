# @author       Jiawei Lu (jiaweil9@asu.edu)
# @time         1/29/23 1:58 PM
# @desc         [script description]


import pandas as pd
import os


network_folder = 'Chicago/chicago_cut'


dtalite_trajectory_df = pd.read_csv(os.path.join(network_folder, r'dtalite_assignment_1_2hour_after_eco_departure/trajectory.csv'))

camlite_agent_list = []
for i in range(len(dtalite_trajectory_df)):
    departure_time = dtalite_trajectory_df.loc[i, 'departure_time']
    node_sequence_str = dtalite_trajectory_df.loc[i, 'node_sequence']
    if node_sequence_str.endswith(';'):
        node_sequence_str = node_sequence_str.rstrip(';')
    node_sequence_list = node_sequence_str.split(';')
    camlite_agent_list.append((dtalite_trajectory_df.loc[i, 'agent_id'],None,None,
                               node_sequence_list[0],node_sequence_list[-1],
                               departure_time,node_sequence_str,
                               dtalite_trajectory_df.loc[i, 'travel_time']))

camlite_agent_df = pd.DataFrame(camlite_agent_list, columns=['agent_id','o_zone_id','d_zone_id','o_node_id','d_node_id','departure_time','route','travel_time_datlite_sim'])
camlite_agent_df.to_csv(os.path.join(network_folder,r'mrm/input_agent_dtalite_after_ecodepart_7-9.csv'), index=False)
