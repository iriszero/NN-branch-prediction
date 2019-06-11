#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

// #define MY_DEBUG
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>

#include "utils.h"
#include <vector>

#define NPERC 16
#define HISTTBL_SIZE NPERC
#define HTBL_SIZE (512*4)
#define PERC_BITS 8
#define THRESHOLD (2*HISTTBL_SIZE + 14)

class Perceptron;
class HistoryTable {
private:
  int array[HISTTBL_SIZE];
  int ptr;
public:
  HistoryTable();
  void push(int n);
  int get_val(size_t idx) const;
};

class NeuralNetwork {
private:
  int weights[NPERC+1];
  int recent_y;
  bool is_within_thr;
public:
  int get_val(size_t idx) const;
  void set_val(size_t idx, int n);
  int pred(const HistoryTable &history_table);
  void update(bool isTaken, const HistoryTable &history_table);
  bool get_is_within_thr(void);
};

class HashTable {
private:
  NeuralNetwork perc[HTBL_SIZE];
  size_t hash(UINT64 idx);
public:
  NeuralNetwork& get(UINT64 idx);
};

class PREDICTOR {

private:
  HistoryTable global_history;
  HashTable hash_perceptron;

public:
  PREDICTOR(void);
  bool    GetPrediction(UINT64 PC);
  void    UpdatePredictor(UINT64 PC, OpType OPTYPE,bool resolveDir, bool predDir, UINT64 branchTarget);
  void    TrackOtherInst(UINT64 PC, OpType opType, bool taken,UINT64 branchTarget);
};

/***********************************************************/
#endif
