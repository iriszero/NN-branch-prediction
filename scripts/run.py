import os
import time
import json
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

modes = ["random", "all", "test", "temp", "short", "long"]

num_parallel_jobs = 4


def run(mode):
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
            if os.path.exists(f'{root_path}/results/{mode}/{k}_{params}'):
                continue

            os.chdir(f'{root_path}/sim')
            try:
                os.unlink(f'predictor.h')
            except:
                pass

            os.symlink(f'pred/predictor_{k}_{params}.h', 'predictor.h')
            os.system('make clean && make -j')

            try:
                os.makedirs(f'{root_path}/results/{mode}/{k}_{params}',
                exist_ok=True)

            except:
                pass

            os.chdir(f'{root_path}/scripts')

            ###################################################################
            start_time = time.time()
            os.system(f'./runall.pl -s ../sim/predictor -w {mode} \
            -f {num_parallel_jobs} -d ../results/{mode}/{k}_{params}')
            elapsed_time = time.time() - start_time

            execution_time[k][params] = elapsed_time
            ###################################################################

            os.system(f'./getdata.pl -w {mode} -d ../results/{mode}/{k}_{params}')
            ###################################################################
            proc=Popen(f'./getamean.pl -w {mode} -d ../results/{mode}/{k}_{params}',
            shell=True, stdout=PIPE, )
            output=proc.communicate()[0]
            _ = float(output.decode("utf-8") )
            amean[k][params] = _
            ###################################################################

    with open(f'{root_path}/results/{mode}/execution_time.json', 'w+') as v:
        v.write(json.dumps(execution_time))


    with open(f'{root_path}/results/{mode}/amean.json', 'w+') as v:
        v.write(json.dumps(amean))


def main():
    for mode in modes:
        run(mode)

if __name__ == "__main__":
    main()
