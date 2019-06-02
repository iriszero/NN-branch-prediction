#include "predictor.h"
#include <math.h>
// Code is derived from P.Michaud and  A. Seznec code for the CBP4 winner
//Sorry two very different code writing styles



/////////////// STORAGE BUDGET JUSTIFICATION ////////////////

// 8KB

/////////////////////////////////////////////////////////////
HistoryTable::HistoryTable() {
  ptr= 0;
  isWithinThreshold = true;
}

void HistoryTable::push(int n) {
  array[ptr++] = n;
  if (ptr >= HISTTBL_SIZE) {
    ptr -= HISTTBL_SIZE;
  }
}

bool HistoryTable::chk_within_threshold() {
  return isWithinThreshold;
}

bool HistoryTable::pred(const Perceptron &perc) {

  int y = 0;
  for (int i=0; i<HISTTBL_SIZE; i++) {
    y += perc.get_val(i) * array[(i+ptr)%HISTTBL_SIZE];
  }

  // Bias (w0)
  y+=perc.get_val(HISTTBL_SIZE);


  isWithinThreshold = (-THRESHOLD <= y && y <= THRESHOLD);

  bool res = (y>=0);

  //printf("pred : %s\n", res?"true":"false");
  return y>=0;
}
int HistoryTable::get_val(size_t idx) const {
  return array[(idx + ptr) % HISTTBL_SIZE];
}

/////////////////////////////////////////////////////////////
int Perceptron::get_val(size_t idx) const {
  assert(0<=idx && idx < NPERC+1);
  return weights[idx];
}
void Perceptron::set_val(size_t idx, int n) {
  assert(0<=idx && idx < NPERC+1);
  if (n >= (1<<PERC_BITS)) {
    n = (1<<PERC_BITS) - 1;
  } else if (n < -(1<<PERC_BITS)) {
    n = -(1<< PERC_BITS);
  }
  weights[idx] = n;
}
void Perceptron::update(bool isTaken, const HistoryTable &history_table) {
  
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

  //printf("Perceptron weights: ");
  for (int i=0; i<=NPERC; i++) {
    //printf("%-3d", weights[i]);
  }
  //printf("\n");
}

/////////////////////////////////////////////////////////////

size_t HashTable::hash(UINT64 idx) {
  size_t h_val = (idx ^ (idx >> 2)) % HTBL_SIZE;
  //printf("Hash value: %u\n", h_val);
  return h_val;
}
Perceptron& HashTable::get(UINT64 idx) {
  return perc[hash(idx)];
}


/////////////////////////////////////////////////////////////
PREDICTOR::PREDICTOR (void)
{

}

bool
PREDICTOR::GetPrediction (UINT64 PC)
{
    return global_history.pred(hash_perceptron.get(PC));
}

void
PREDICTOR::UpdatePredictor (UINT64 PC, OpType OPTYPE, bool resolveDir,
			    bool predDir, UINT64 branchTarget)
{
  if ((resolveDir != predDir) ||
    (global_history.chk_within_threshold() == true) ) {

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
