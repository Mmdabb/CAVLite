# @author       Jiawei Lu (jiaweil9@asu.edu)
# @time         1/30/23 2:36 PM
# @desc         [script description]


import pandas as pd
import os
import math


working_folder = r'/Users/jiawei/Desktop/anl'
camlite_agent_filepath = os.path.join(working_folder, 'mrm_7-9_withlp_agent14.csv')

aggregation_time_interval = 15

camlite_agent_df = pd.read_csv(camlite_agent_filepath, nrows=None)

# camlite_agent_df_simplified = camlite_agent_df[['agent_id','from_origin_node_id_meso','to_destination_node_id_meso','departure_time_in_min',
#                                 'arrival_time_in_min','complete_flag','travel_time_in_min','meso_node_sequence']]
# camlite_agent_df_simplified.to_csv(os.path.join(working_folder, 'agent_6-8_simplified.csv'), index=False)


meso_node_df = pd.read_csv(r'Chicago/chicago_cut/mrm/mesonet/node.csv', dtype={'zone_id':'Int64'})
node_zone_dict = dict(zip(meso_node_df['node_id'], meso_node_df['zone_id']))


min_departure_time = camlite_agent_df['departure_time_in_min'].min()
max_departure_time = camlite_agent_df['departure_time_in_min'].max()
time_begin = math.floor(min_departure_time / aggregation_time_interval) * aggregation_time_interval
time_end = math.ceil(max_departure_time / aggregation_time_interval) * aggregation_time_interval
time_intervals = list(range(time_begin, time_end, aggregation_time_interval))
number_of_time_intervals = len(time_intervals)

travel_time_dict = {}
for i in range(len(camlite_agent_df)):
    o_d = (node_zone_dict[camlite_agent_df.loc[i,'from_origin_node_id_meso']], node_zone_dict[camlite_agent_df.loc[i,'to_destination_node_id_meso']])
    departure_time_interval = min(math.floor((camlite_agent_df.loc[i,'departure_time_in_min'] - time_begin) / aggregation_time_interval), number_of_time_intervals-1)
    if o_d not in travel_time_dict.keys():
        travel_time_dict[o_d] = {'count_total': [0] * number_of_time_intervals, 'count_finished': [0] * number_of_time_intervals, 'td_travel_time_sum': [0.0] * number_of_time_intervals}

    travel_time_dict[o_d]['count_total'][departure_time_interval] += 1
    if not pd.isna(camlite_agent_df.loc[i,'travel_time_in_min']):
        travel_time_dict[o_d]['count_finished'][departure_time_interval] += 1
        travel_time_dict[o_d]['td_travel_time_sum'][departure_time_interval] += camlite_agent_df.loc[i,'travel_time_in_min']

od_tt_list = []
for o_d, o_d_tt_dict in travel_time_dict.items():
    od_tt_info = [*o_d, '-'.join(map(str, o_d))]
    travel_time_sum = 0.0
    count_finished_sum = 0
    count_total_sum = 0
    for k in range(number_of_time_intervals):
        count_total_sum += o_d_tt_dict['count_total'][k]
        if o_d_tt_dict['count_finished'][k] > 0:
            od_tt_info.append(round(o_d_tt_dict['td_travel_time_sum'][k]/o_d_tt_dict['count_finished'][k], 2))
            travel_time_sum += o_d_tt_dict['td_travel_time_sum'][k]
            count_finished_sum += o_d_tt_dict['count_finished'][k]
        else:
            od_tt_info.append(None)

    if count_finished_sum > 0:
        od_tt_info.append(round(travel_time_sum / count_finished_sum, 2))
    else:
        od_tt_info.append(None)

    od_tt_info += [count_finished_sum, count_total_sum]

    od_tt_list.append(od_tt_info)


time_intervals_str = []
for time_interval in time_intervals:
    hh = math.floor(time_interval / 60)
    mm = time_interval - hh * 60
    hh_str = str(hh) if hh >= 10 else '0'+str(hh)
    mm_str = str(mm) if mm >= 10 else '0'+str(mm)
    time_intervals_str.append(hh_str+mm_str)

column_names = ['o_zone_id','d_zone_id', 'od'] + time_intervals_str + ['average_tt','volume_finished', 'volume']
od_tt_df = pd.DataFrame(od_tt_list, columns=column_names)
od_tt_df.to_csv(os.path.join(working_folder, 'statistics_camlite_od_travel_time_before_withlp_14.csv'), index=False)
