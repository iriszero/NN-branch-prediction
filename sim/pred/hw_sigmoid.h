#ifndef _HW_SIGMOID_H_
#define _HW_SIGMOID_H_


#include <math.h>
#include <assert.h>

class HW_Sigmoid {
private:
  size_t bits;
  int* lookup_table;
  double sigmoid(int x);
public:
  HW_Sigmoid(size_t bits);
  ~HW_Sigmoid() {}
  int get_val(int n);
private:
  void pre_compute();
};


#endif
