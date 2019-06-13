#ifndef _HW_SIGMOID_H_
#define _HW_SIGMOID_H_


#include <math.h>
#include <assert.h>

double HW_Sigmoid::sigmoid(int x) {
    return 1 / (1 + exp(x));
}
HW_Sigmoid::HW_Sigmoid(size_t bits) : this->bits(bits) {
    lookup_table = new int[1 << bits];
    pre_compute();
}
HW_Sigmoid::~HW_Sigmoid() {
  delete[] lookup_table;
}
int HW_Sigmoid::get_val(int n) {
  assert(-(1 << (bits-1)) <= n && n < (1 << (bits-1)));
  return lookup_table[n + ( 1 << (bits-1))]
}
HW_Sigmoid::pre_compute() {
  for (int i=0; i<(1<<bits); i++) {

  }
}
#endif
