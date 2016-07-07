'''This contains functions for operating on state vectors.'''

import sys
import math
import numpy as np
from PIL import Image
#Layout of 6502
#PC + Acc + X + Y + SP + SR = 2 + 1 + 1 + 1 + 1 + 1 = 7 bytes of regs
#2^16 bytes mem
#65,543 bytes total = 524,344 bits
sz_st_vec_bytes = 7+1 + 2**16 #This +1 is due to me not understanding the state.
#sz_st_vec_bits = sz_st_vec_bytes * 8

def path_to_svarr(path):
    trc = np.fromfile(path, dtype='uint8')
    num_states = trc.size / (sz_st_vec_bytes)

    if num_states != math.floor(num_states):
        print("num_states is non int!")
        sys.exit()

    return np.reshape(trc, (int(num_states), -1))

def paths_to_tracearr(paths):
    return np.stack([ path_to_svarr(path) for path in paths ])

def show_arr(arr, grey=245):
    return Image.fromarray(grey * np.uint8(arr))

'''
def dir_to_arr_labels(path, lab):
    #All of the traces.
    image_list = []
    #Training labels.
    labels = []
    files = !ls {path}
    #Get images as 2d arrays as well as labels.
    for filename in files:
        im=misc.imread(path + filename, flatten=True)
        image_list.append(np.array(im)/255)
        labels.append(lab)
    return np.array(image_list), np.array(labels)
'''
