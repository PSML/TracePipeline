'''This contains functions for operating on state vectors.'''
import sys
import math
import numpy as np
from PIL import Image
#Layout of 6502
#PC + Acc + X + Y + SP + SR = 2 + 1 + 1 + 1 + 1 + 1 = 7 bytes of regs
#2^16 bytes mem
#65,543 bytes total = 524,344 bits
sz_st_vec_bytes = 8 + 2**16 

def path_to_svarr(path):
    #Takes a path to a trace and returns a np array.
    trc = np.fromfile(path, dtype='uint8')
    num_states = trc.size / (sz_st_vec_bytes)

    if num_states != math.floor(num_states):
        print("num_states is non int!")
        sys.exit()

    return np.reshape(trc, (int(num_states), -1))

def sv_paths_to_tracearr(paths):
    #Takes a list of paths and turns into a 3d numpy array.
    #Eats lots of memory with sparse svs.
    return np.stack([ path_to_svarr(path) for path in paths ])

def show_arr(arr, grey=245):
    #Visual representation of trace. Careful w/ bit vs byte.
    return Image.fromarray(grey * np.uint8(arr))

def excited_cols(trc):
    #Takes a trace, returns excited cols.
    #Works with bit or byte representation.
    return (trc!=trc[0,:]).any(0)

def paths_to_datadep_mask(paths):
    #Takes a trace, returns cols that vary accross the dataset.
    #The reason this doesn't just use sum(axis=0) is because
    #It is assumed only ~1 trace can fit in mem at a given time.
    #Kludge
    sum = np.zeros(path_to_svarr(paths[0]).shape)
    for i in paths:
        sum += path_to_svarr(i)
    #Mask out only the differences between traces.
    return ((sum > 0) & (sum < len(paths))).any(0)

def paths_to_zero_cols(paths):
    trc_sum = np.zeros(path_to_svarr(paths[0]).shape).astype('uint64')
    for i in paths:
        trc_sum += path_to_svarr(i).sum(0)

    #Mask out only the differences between traces.
    return (sum > 0) & (sum < len(paths))
    

def paths_to_excited_mask(paths):
    #Takes a list of paths to traces. Returns the excited bit mask
    #across all traces in list.
    return np.array( [excited_cols(path_to_svarr(i)) for i in paths] ).any(0)

def non_zero_cols(trc):
    return np.all(trc, axis=0)

def gen_non_zero_mask(trc_list):
    return np.array( [non_zero_cols(path_to_svarr(i)) for i in trc_list] ).any(0)

#Combine these into 1 fn combine gen fns
def path_to_svmasked(path, mask):
    #Simply masks out path.
    return path_to_svarr(path)[:,mask]

def paths_to_svmasked(paths, mask):
    #List of paths and mask to 3d array of masked traces
    return np.array( [ path_to_svmasked(i, mask) for i in paths ] )

def mask_svarr(trcs, mask):
    #Turns 3d array of traces to 3d array selected with mask.
    return np.array( [ i[:,mask] for i in trcs ] )

def svarr_to_excited_mask(trcs):
    #3d array to excited mask
    return np.array( [ excited_cols(i) for i in trcs ] ).any(0)

def byte_to_bit_sv(trc):
    #Converts byte traces to bit traces.
    #Likely need to remask after running.
    return np.unpackbits(trc, axis=2)


