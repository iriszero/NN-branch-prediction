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
modes = ["test", "temp", "random", ] # "short", "long", "all"]

num_parallel_jobs = 4


def run(mode):
    root_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        os.pardir
    )

    amean = {}
    for k, v in methods.items():
        amean[k] = {}

        os.chdir(f'{root_path}/sim')

        try:
            os.unlink(f'predictor.cc')
        except:
            pass

        os.symlink(f'pred/predictor_{k}.cc', 'predictor.cc')

        for params in v:
            os.chdir(f'{root_path}/sim')
            try:
                os.unlink(f'predictor.h')
            except:
                pass

            os.symlink(f'pred/predictor_{k}_{params}.h', 'predictor.h')
            # os.system('make clean && make -j')

            try:
                os.makedirs(f'{root_path}/results/{mode}/{k}_{params}',
                exist_ok=True)

            except:
                pass

            os.chdir(f'{root_path}/scripts')

            ###################################################################
            proc=Popen(f'./getamean.pl -w {mode} -d ../results/{mode}/{k}_{params}',
            shell=True, stdout=PIPE, )
            output=proc.communicate()[0]
            _ = float(output.decode("utf-8") )
            amean[k][params] = _
            ###################################################################



    with open(f'{root_path}/results/{mode}/amean.json', 'w+') as v:
        v.write(json.dumps(amean))

def main():
    for mode in modes:
        run(mode)

if __name__ == "__main__":
    main()
