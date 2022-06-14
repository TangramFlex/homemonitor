import numpy.ctypeslib as ctl
import ctypes
import os
from enum import Enum

# declare the same enums that are used in the c code for convenience
class Motion(Enum):
    Active = 0
    Inactive = 1
    SensorError = 2

class Entry(Enum):
    Open = 0
    Closed = 1
    SensorError = 2

# Load Library
# in case there is doubt as to if the shared lib exists, uncomment below
# print(os.listdir(lib_dir))
transform_lib=ctl.load_library('libtransformmsg', './')
# print(transform_lib)

# function declarations from the shared library
# most of these are just to make the names in python shorter
create_input_msg = transform_lib.create_and_populate_input_msg
# type checking on arguments
create_input_msg.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
output_msg = transform_lib.create_output_msg
my_transform = transform_lib.transform
# all the output string functions and the return declarations
my_output_l1 = transform_lib.get_living_room_motion_1_output_string
my_output_l1.restype = ctypes.c_char_p
my_output_l2 = transform_lib.get_living_room_motion_2_output_string
my_output_l2.restype = ctypes.c_char_p
my_output_ge = transform_lib.get_garage_door_entry_output_string
my_output_ge.restype = ctypes.c_char_p
my_output_fe = transform_lib.get_front_door_entry_output_string
my_output_fe.restype = ctypes.c_char_p
my_output_b1 = transform_lib.get_basement_motion_1_output_string
my_output_b1.restype = ctypes.c_char_p
my_output_pt = transform_lib.get_patio_temp_output_string
my_output_pt.restype = ctypes.c_char_p

# note: I am sure it would have been easier if I could pass a whole object back, but I ran out of time
def gather_outputs(output):
    l1_output = my_output_l1(output).decode('utf-8')
    l2_output = my_output_l2(output).decode('utf-8')
    ge_output = my_output_ge(output).decode('utf-8')
    fe_output = my_output_fe(output).decode('utf-8')
    b1_output = my_output_b1(output).decode('utf-8')
    pt_output = my_output_pt(output).decode('utf-8')

    # combine all the output strings to make a representation of the otuput msg
    # not pretty, but it works for this demo
    return "{\n\tliving_room_motion_1:" + l1_output + "\n" +\
                "\tliving_room_motion_2:" + l2_output + "\n" + \
                "\tgarage_door_entry:" + ge_output + "\n" + \
                "\tfront_door_entry:" + fe_output + "\n" + \
                "\tbasement_motion_1:" + b1_output + "\n" + \
                "\tpatio_temp:" + pt_output + "\n" + \
                "}"

def create_input_message(living_room_motion_1, living_room_motion_2, garage_door_entry, front_door_entry, basement_motion_1, patio_temp):
    my_input = create_input_msg(
        ctypes.c_int(living_room_motion_1.value),
        ctypes.c_int(living_room_motion_2.value),
        ctypes.c_int(garage_door_entry.value),
        ctypes.c_char_p(front_door_entry.encode('utf-8')),
        ctypes.c_int(basement_motion_1),
        ctypes.c_int(patio_temp),
    )

    return my_input

def old2new(living_room_motion_1, living_room_motion_2, garage_door_entry, front_door_entry, basement_motion_1, patio_temp):
    my_input = create_input_message(living_room_motion_1, living_room_motion_2, garage_door_entry, front_door_entry, basement_motion_1, patio_temp)
    # print("After my_input declaration")
    my_output = output_msg()
    # print("After output declaration")
    my_workable_output = ""
    my_transform_success = my_transform(my_input, my_output)
    # print("my_transform_success:", my_transform_success)
    if my_transform_success == 0:
        # print("transform succeeded!")
        my_workable_output = gather_outputs(my_output)
    else:
        print("transform failed...")
    
    return my_workable_output

living_room_motion_1 = Motion.Inactive
living_room_motion_2 = Motion.Active
garage_door_entry = Entry.Open
front_door_entry = "Closed"
basement_motion_1 = 0
patio_temp = 75

print("New Message Body:", old2new(living_room_motion_1, living_room_motion_2, garage_door_entry, front_door_entry, basement_motion_1, patio_temp))