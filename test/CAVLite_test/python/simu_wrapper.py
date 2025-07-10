# simu_wrapper.py
import ctypes
import os
from ctypes import Structure, c_int, c_float, c_char_p, POINTER

# Load the DLL
dll_path = os.path.abspath("CAVLite.dll")
cavlite = ctypes.CDLL(dll_path)

# Define AgentRawInput structure (must match C++ struct)
class AgentRawInput(Structure):
    _fields_ = [
        ("agent_id", c_int),
        ("o_node_id", c_int),
        ("d_node_id", c_int),
        ("o_zone_id", c_int),
        ("d_zone_id", c_int),
        ("volume", c_float),
        ("departure_time", c_float),
        ("node_sequence", c_char_p),
    ]

# Define pointer to SimulationAPI as void*
SimulationPtr = ctypes.c_void_p

# Use the C-style wrapper function names
cavlite.create_simulation.restype = SimulationPtr

cavlite.simulation_initialize.argtypes = [SimulationPtr,
                                          POINTER(c_int),
                                          POINTER(c_int),
                                          POINTER(c_int)]
cavlite.simulation_initialize.restype = ctypes.c_bool

cavlite.simulation_load_and_run_step.argtypes = [SimulationPtr, c_int,
                                                 POINTER(AgentRawInput), c_int]
cavlite.simulation_finalize.argtypes = [SimulationPtr]
cavlite.destroy_simulation.argtypes = [SimulationPtr]


class Simu:
    def __init__(self):
        self.obj = cavlite.create_simulation()
        self.start = c_int()
        self.end = c_int()
        self.step = c_int()
        success = cavlite.simulation_initialize(self.obj,
                                                ctypes.byref(self.start),
                                                ctypes.byref(self.end),
                                                ctypes.byref(self.step))
        if not success:
            raise RuntimeError("Simulation initialization failed")

    def run_step(self, t: int, agents):
        agent_arr = (AgentRawInput * len(agents))(*agents)
        cavlite.simulation_load_and_run_step(self.obj, t, agent_arr, len(agents))

    def finalize(self):
        cavlite.simulation_finalize(self.obj)
        cavlite.destroy_simulation(self.obj)
