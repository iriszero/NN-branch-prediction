#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

//#define MY_DEBUG
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>

#include "utils.h"
#include <vector>


//// Neural Network Setting ////
#define NN_D 30
#define NN_N 4
////////////////////////////////

#define HISTTBL_SIZE NN_D
#define HTBL_SIZE 256
#define PERC_BITS 8
#define THRESHOLD ((2*HISTTBL_SIZE + 14) * (NN_N + 1))

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
  /* w1: [0-d][1-n] is actually used in HW implementation */
  int w1[NN_D+1][NN_N+1];
  int w2[NN_N+1];

  /* Careful! This needs to be initialzed as zero for every iteration. It is
  temporary stored for the update()*/
  int h[NN_N+1];

  int recent_y;
  bool is_within_thr;
  int make_within_range(int n) const;
public:
  NeuralNetwork();
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
