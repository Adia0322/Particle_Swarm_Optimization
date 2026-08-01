import ctypes
import numpy as np
import cv2
import os

class DICProcessor:
    def __init__(self, dll_path='./build/PSO.dll'):
        if not os.path.exists(dll_path):
            raise FileNotFoundError(f"DLL not found at {dll_path}")
        
        self.lib = ctypes.CDLL(dll_path)
        self._setup_argtypes()

    def _setup_argtypes(self):
        self.lib.pso_optimize.restype = None
        self.lib.pso_optimize.argtypes = [
            ctypes.POINTER(ctypes.c_double), # ref_img
            ctypes.POINTER(ctypes.c_double), # cur_img
            ctypes.c_int,                    # width
            ctypes.c_int,                    # height
            ctypes.c_int,                    # population
            ctypes.c_int,                    # subset_side_len
            ctypes.POINTER(ctypes.c_double), # img_ref_pt
            ctypes.POINTER(ctypes.c_double), # img_cur_pt
            ctypes.POINTER(ctypes.c_double)  # result_buffer
        ]

    def run(self, img_ref, img_cur, ref_pt, cur_pt_init, population=40, subset_len=31):

        img_ref_f = np.ascontiguousarray(img_ref, dtype=np.double)
        img_cur_f = np.ascontiguousarray(img_cur, dtype=np.double)
        h, w = img_ref_f.shape

        img_ref_f_ptr = img_ref_f.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
        img_cur_f_ptr = img_cur_f.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

        img_ref_pt_arr = np.array(ref_pt, dtype=np.double)
        img_cur_pt_arr = np.array(cur_pt_init, dtype=np.double)
        img_ref_pt_arr_ptr = img_ref_pt_arr.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
        img_cur_pt_arr_ptr = img_cur_pt_arr.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

        result_buffer = np.zeros(3, dtype=np.double)
        result_buffer_ptr = result_buffer.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

        self.lib.pso_optimize(
            img_ref_f_ptr, 
            img_cur_f_ptr, 
            w, 
            h, 
            population,
            subset_len,
            img_ref_pt_arr_ptr,
            img_cur_pt_arr_ptr,
            result_buffer_ptr
        )
        result_y = result_buffer[0]
        result_x = result_buffer[1]
        coef  = result_buffer[2]

        return {
            'y': result_buffer[0],
            'x': result_buffer[1],
            'coef': result_buffer[2]
        }
    
if __name__ == "__main__":
    proc = DICProcessor(dll_path='./build/PSO.dll')

    img1                    = cv2.imread('image/img1.jpg', 0)
    img2                    = cv2.imread('image/img1_shefted_5_5.jpg', 0)

    ref_x, ref_y            = 426, 320
    start_x, start_y        = 426, 320
    result                  = proc.run(img1, img2, [ref_y, ref_x], [start_y, start_x])
    print(f"Result: x={result['x']}, y={result['y']}, coefficient={result['coef']}")

