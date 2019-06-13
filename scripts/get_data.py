import datetime
import multiprocessing
import json
import os
import time

from subprocess import Popen,PIPE,STDOUT,call


methods = {
    'all_true' : [''],
    'all_false' : [''],
    "perceptron" : ['8kb_8', '8kb_16', '8kb_32', '8kb_64', '8kb_128',
    '32kb_8', '32kb_16', '32kb_32', '32kb_64', '32kb_128'
    ],
    "2layer" : ['8kb_30_1', '8kb_30_2', '8kb_30_4',
    '32kb_30_1', '32kb_30_2', '32kb_30_4', '32kb_30_8',
    '32kb_62_1','32kb_62_2','32kb_62_4','32kb_62_8',
    ],
    "rnn" : [],
    "michaud" : [''],
}

modes = ["all", "short", "long", "random", "test", "temp"]

def run(mode):
    num_parallel_jobs = multiprocessing.cpu_count()
    root_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        os.pardir
    )

    execution_time = {}
    amean = {}

    for k, v in methods.items():
        execution_time[k] = {}
        amean[k] = {}

        os.chdir(f'{root_path}/sim')

        try:
            os.unlink(f'predictor.cc')
        except:
            pass

        os.symlink(f'pred/predictor_{k}.cc', 'predictor.cc')

        for params in v:
            os.chdir(f'{root_path}/scripts')

            os.system(f'./getdata.pl -w {mode} -d ../results/{mode}/{k}_{params}')
            

def main():
    for mode in modes:
        print('#' * 50)
        print(f'[mode] {mode}')
        run(mode)
        print('#' * 50)

if __name__ == "__main__":
    main()
