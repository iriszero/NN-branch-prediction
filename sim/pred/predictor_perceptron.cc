#include "predictor.h"
#include <math.h>
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
int NeuralNetwork::get_val(size_t idx) const {
  assert(0<=idx && idx < NPERC+1);
  return weights[idx];
}
void NeuralNetwork::set_val(size_t idx, int n) {
  assert(0<=idx && idx < NPERC+1);
  if (n >= (1<<PERC_BITS)) {
    n = (1<<PERC_BITS) - 1;
  } else if (n < -(1<<PERC_BITS)) {
    n = -(1<< PERC_BITS);
  }
  weights[idx] = n;
}
int NeuralNetwork::pred(const HistoryTable &history_table) {
  int y = 0;
  for (int i=0; i<HISTTBL_SIZE; i++) {
    y += get_val(i) * history_table.get_val(i);
  }

  // Bias
  y +=get_val(HISTTBL_SIZE);
  is_within_thr = (-THRESHOLD <= y && y <= THRESHOLD);

#ifdef MY_DEBUG
  printf("y =%d\n", y);
#endif

  return y>=0;
}
void NeuralNetwork::update(bool isTaken, const HistoryTable &history_table) {

  for (int i=0; i<NPERC; i++) {
    if (history_table.get_val(i) == (isTaken? 1 : -1)) {
      set_val(i, weights[i] + 1);
    } else {
      set_val(i, weights[i] - 1);
    }
  }

  if ((isTaken?1:-1) == 1) {
    set_val(NPERC, weights[NPERC] + 1);
  } else {
    set_val(NPERC, weights[NPERC] - 1);
  }

#ifdef MY_DEBUG
  printf("Perceptron weights: ");
  for (int i=0; i<=NPERC; i++) {
    printf("%-4d", weights[i]);
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
