#include "predictor.h"
#include <math.h>
#include <random>
// Code is derived from P.Michaud and  A. Seznec code for the CBP4 winner
//Sorry two very different code writing styles



/////////////// STORAGE BUDGET JUSTIFICATION ////////////////
// 8KB, 32KB, Unlimited
/////////////////////////////////////////////////////////////
HistoryTable::HistoryTable() {
  ptr= 0;
}

void HistoryTable::push(int n) {
  array[ptr++] = n;
  if (ptr >= HISTTBL_SIZE) {
    ptr -= HISTTBL_SIZE;
  }
}

int HistoryTable::get_val(size_t idx) const {
  return array[(idx + ptr) % HISTTBL_SIZE];
}

/////////////////////////////////////////////////////////////
NeuralNetwork::NeuralNetwork() {
  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0, 1 << (PERC_BITS / 2));

  for (int i=0; i<=NN_D; i++) {
    for (int j=0; j<=NN_N; j++) {
      w1[i][j] = int(distribution(generator));
    }
  }
  for (int i=0; i<=NN_N; i++) {
    w2[i] = int(distribution(generator));
  }
}
int NeuralNetwork::make_within_range(int n) const {
  if (n >= (1<<(PERC_BITS-1))) {
    n = (1<<(1<<(PERC_BITS-1)) - 1;
  } else if (n < -(1<<(PERC_BITS-1)) {
    n = -(1<< (1<<(PERC_BITS-1));
  }
  return n;
}

int NeuralNetwork::pred(const HistoryTable &history_table) {
  for (int j=0; j<NN_N+1;j++) {
    h[j] = 0;
  }

  for (int j=1; j<=NN_N; j++) {
    // bias
    h[j] = make_within_range(
      h[j] + w1[0][j] * 1
    );

    for (int i=0; i<HISTTBL_SIZE; i++) {
      h[j] = make_within_range(
        h[j] + w1[i+1][j] * history_table.get_val(i)
      );
    }
  }

  int y = 0;

  //bias
  y+= w2[0] * 1;

  for (int j=1; j<=NN_N; j++) {
    y += w2[j] * h[j];
  }

  is_within_thr = (-THRESHOLD <= y && y <= THRESHOLD);

#ifdef MY_DEBUG
  printf("y =%d\n", y);
#endif

  return y>=0;
}

void NeuralNetwork::update(bool isTaken, const HistoryTable &history_table) {
  for (int j=1; j<=NN_N; j++) {
    w1[0][j] = make_within_range(
      w1[0][j] - h[j]
    );

    for (int i=0; i<HISTTBL_SIZE; i++) {
      w1[i+1][j] = make_within_range(
        w1[i+1][j] - h[j] * history_table.get_val(i)
      );
    }
  }

  // bias
  w2[0] = make_within_range(
    w2[0] + (isTaken? 1: -1)
  );


  for (int j=1; j<=NN_N; j++) {
    w2[j] = make_within_range(
      w2[j] - h[j]
    );
  }

#ifdef MY_DEBUG
  printf("w1 : \n");
  for (int i=0; i<= NN_D; i++) {
    for (int j=0; j<= NN_N; j++) {
      printf("%-6d", w1[i][j]);
    }
    printf("\n");
  }

  printf("w2 : \n");
  for (int j=0; j<=NN_N; j++) {
    printf("%-6d", w2[j]);
  }
  printf("\n");

  printf("Hidden weights: \n");
  for (int j=1; j<=NN_N; j++) {
    printf("%-6d", h[j]);
  }
  printf("\n");
#endif

}
bool NeuralNetwork::get_is_within_thr(void) {
  return is_within_thr;
}
/////////////////////////////////////////////////////////////

size_t HashTable::hash(UINT64 idx) {
  size_t h_val = (idx ^ (idx >> 2)) % HTBL_SIZE;

  return h_val;
}
NeuralNetwork& HashTable::get(UINT64 idx) {
  return perc[hash(idx)];
}


/////////////////////////////////////////////////////////////
PREDICTOR::PREDICTOR (void)
{

}

bool
PREDICTOR::GetPrediction (UINT64 PC)
{
    return hash_perceptron.get(PC).pred(global_history);
}

void
PREDICTOR::UpdatePredictor (UINT64 PC, OpType OPTYPE, bool resolveDir,
			    bool predDir, UINT64 branchTarget)
{
  if ((resolveDir != predDir) ||
    (hash_perceptron.get(PC).get_is_within_thr() == true) ) {

    hash_perceptron.get(PC).update(resolveDir, global_history);
  }

  global_history.push(resolveDir? 1: -1);
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void
PREDICTOR::TrackOtherInst (UINT64 PC, OpType opType, bool taken,
			   UINT64 branchTarget)
{

}
