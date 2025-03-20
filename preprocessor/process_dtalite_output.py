# @author       Jiawei Lu (jiaweil9@asu.edu)
# @time         1/31/23 4:37 PM
# @desc         [script description]


import pandas as pd
import os


working_folder = r'Chicago/chicago_cut/dtalite_assignment_1_2hour_after'
dtalite_route_filepath = os.path.join(working_folder, 'route_assignment.csv')


dtalite_route_df = pd.read_csv(dtalite_route_filepath)

travel_time_dict = {}
for i in range(len(dtalite_route_df)):
    o_d = (dtalite_route_df.loc[i,'o_zone_id'], dtalite_route_df.loc[i,'d_zone_id'])
    if o_d not in travel_time_dict.keys():
        travel_time_dict[o_d] = {'volume': [], 'travel_time': []}

    travel_time_dict[o_d]['volume'].append(dtalite_route_df.loc[i,'volume'])
    travel_time_dict[o_d]['travel_time'].append(dtalite_route_df.loc[i,'VDF_travel_time'])

od_tt_list = []
for o_d, o_d_tt_dict in travel_time_dict.items():
    od_tt_info = [*o_d, '-'.join(map(str,o_d))]
    total_travel_time = 0.0
    total_count = 0
    for k in range(len(o_d_tt_dict['volume'])):
        total_travel_time += o_d_tt_dict['travel_time'][k] * o_d_tt_dict['volume'][k]
        total_count += o_d_tt_dict['volume'][k]

    od_tt_info.append(round(total_travel_time / total_count, 2))
    od_tt_info.append(round(total_count,2))

    od_tt_list.append(od_tt_info)

column_names = ['o_zone_id','d_zone_id', 'od', 'average_tt','volume']
od_tt_df = pd.DataFrame(od_tt_list, columns=column_names)
od_tt_df.to_csv(os.path.join(working_folder, 'statistics_dtalite_od_travel_time.csv'), index=False)
a = 1