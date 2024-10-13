import sys
import ctypes

sample_lib = ctypes.CDLL(sys.argv[1])

module = {}

sample_lib.setup_python.argtypes = [ctypes.py_object]
sample_lib.setup_python(module)

module["print_string"]("Hello World")

def print_callback(string):
    print("C++ prints: {}".format(string))

module["run_callback"](print_callback)
