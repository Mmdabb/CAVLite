# @author       Jiawei Lu (jiaweil9@asu.edu)
# @time         1/15/23 5:47 PM
# @desc         [script description]


import pandas as pd
import os

import time



network_folder = 'Chicago/chicago_cut'


signal_timing_plan_df = pd.read_csv(os.path.join(network_folder, 'signal_timing_plan.csv'))
signal_phase_mvmt_df = pd.read_csv(os.path.join(network_folder, 'signal_phase_mvmt.csv'))
signal_timing_phase_df = pd.read_csv(os.path.join(network_folder, 'signal_timing_phase.csv'))

link_df = pd.read_csv(os.path.join(network_folder, r'mrm/mesonet/link_without_signal.csv'), dtype={'macro_link_id':'Int64','macro_node_id':'Int64','movement_id':'Int64'})


class Controller:
    def __init__(self, controller_id_, timing_plan_id_, cycle_length_):
        self.controller_id = controller_id_
        self.timing_plan_id = timing_plan_id_
        self.cycle_length = cycle_length_

class TimingPlan:
    def __init__(self, timing_plan_id_):
        self.timing_plan_id = timing_plan_id_
        self.cycle_length = 0
        self.phases = []

    def generatingTiming(self):
        self.phases.sort(key=lambda x: x.phase_num)
        start_time = 0
        for p in self.phases:
            p.start_time = start_time
            p.end_time = start_time + p.total_duration
            start_time = p.end_time


class Phase:
    def __init__(self, phase_num_, duration_, clearance_):
        self.phase_num = phase_num_
        self.duration = duration_
        self.clearance = clearance_
        self.movements = []
        self.start_time = 0
        self.end_time = 0
    @property
    def total_duration(self):
        return self.duration + self.clearance


controller_dict = {}
timing_plan_dict = {}

for i in range(len(signal_timing_plan_df)):
    controller = Controller(signal_timing_plan_df.loc[i,'controller_id'], signal_timing_plan_df.loc[i,'timing_plan_id'], signal_timing_plan_df.loc[i,'cycle_length'])
    controller_dict[controller.controller_id] = controller

for i in range(len(signal_timing_phase_df)):
    timing_plan_id = signal_timing_phase_df.loc[i,'timing_plan_id']
    if timing_plan_id not in timing_plan_dict.keys():
        timing_plan = TimingPlan(timing_plan_id)
        timing_plan_dict[timing_plan_id] = timing_plan
    else:
        timing_plan = timing_plan_dict[timing_plan_id]
    phase = Phase(signal_timing_phase_df.loc[i,'signal_phase_num'], signal_timing_phase_df.loc[i,'min_green'], signal_timing_phase_df.loc[i,'clearance'])
    timing_plan.phases.append(phase)

for _, controller in controller_dict.items():
    if controller.timing_plan_id in timing_plan_dict.keys():
        timing_plan_dict[controller.timing_plan_id].cycle_length = controller.cycle_length
for _, timing_plan in timing_plan_dict.items():
    timing_plan.generatingTiming()

for i in range(len(signal_phase_mvmt_df)):
    controller_id = signal_phase_mvmt_df.loc[i,'controller_id']
    signal_phase_num = signal_phase_mvmt_df.loc[i,'signal_phase_num']
    mvmt_id = signal_phase_mvmt_df.loc[i,'mvmt_id']
    phase = timing_plan_dict[controller_dict[controller_id].timing_plan_id].phases[signal_phase_num-1]
    assert phase.phase_num == signal_phase_num
    phase.movements.append(mvmt_id)

mvmt_dict = {}
for _, timing_plan in timing_plan_dict.items():
    for phase in timing_plan.phases:
        for mvmt_id in phase.movements:
            mvmt_dict[mvmt_id] = {'cycle_length':timing_plan.cycle_length, 'start_green_time':phase.start_time, 'end_green_time':phase.end_time}


# ============== DTALite=============== #
link_signal_info_list = []
for i in range(len(link_df)):
    mvmt_id = link_df.loc[i,'movement_id']
    if mvmt_id in mvmt_dict.keys():
        link_signal_info_list.append(mvmt_dict[mvmt_id])
    else:
        link_signal_info_list.append({})
link_signal_info_df = pd.DataFrame(link_signal_info_list, dtype='Int64')

link_df = pd.concat([link_df,link_signal_info_df], axis=1)
link_df.to_csv(os.path.join(network_folder, r'mrm/mesonet/link.csv'), index=False)


# ============== CAMLite=============== #
# mvmt_to_link_dict = {}
# for i in range(len(link_df)):
#     mvmt_id = link_df.loc[i,'movement_id']
#     if pd.isna(mvmt_id): continue
#     mvmt_to_link_dict[mvmt_id] = link_df.loc[i,'link_id']
#
# camlite_signal_info_list = []
# for _, timing_plan in timing_plan_dict.items():
#     for phase in timing_plan.phases:
#         camlite_signal_info_list.append((timing_plan.timing_plan_id, phase.phase_num-1, ';'.join(map(lambda x: str(mvmt_to_link_dict[x]), phase.movements)), phase.total_duration))
# camlite_signal_info_df = pd.DataFrame(camlite_signal_info_list, columns=['controller_id','phase_id','movement_links','duration'])
# camlite_signal_info_df.to_csv(os.path.join(network_folder, r'mrm/input_signal.csv'), index=False)




a = 1