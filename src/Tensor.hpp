#ifndef TENSOR_HPP
#define TENSOR_HPP

#include "mpm_conf.hpp"
#include "Eigen/Dense"
#include "Eigen/Core"
#include <vector>

class Tensor {
  uint dim;
  std::vector<FLOAT> t;

public: 
  Tensor(FLOAT d = 3);
  Tensor(MAT3 M);
  FLOAT& operator() (int i, int j, int k, int l);
  const FLOAT& operator()(int i, int j, int k, int l) const;

  // Tensor inverse() const;
  // Tensor inverseOrtho() const;

  friend MATX tensor2Mat(const Tensor & T);
  friend Tensor mat2Tensor(const MATX & M);

  friend MATX tensor2MatOrtho(const Tensor & T);
  friend Tensor mat2TensorOrtho(const MATX & M);

  friend MAT3 innerProduct(const Tensor & T, const MAT3 & M);
  friend MAT3 innerProduct(const MAT3 & M, const Tensor & T);
  friend Tensor innerProduct(const Tensor & A, const Tensor & B);

  friend std::ostream& operator<<(std::ostream& os, const Tensor& T);

  friend Tensor rotateTensor(const Tensor & T, const MAT3 & R);
  friend Tensor transformTensor(const Tensor & T, const MAT3 & M);
};

MATX tensor2Mat(const Tensor & T);
Tensor mat2Tensor(const MATX & M);

MATX tensor2MatOrtho(const Tensor & T);
Tensor mat2TensorOrtho(const MATX & M);

MAT3 innerProduct(const Tensor & T, const MAT3 & M);
MAT3 innerProduct(const MAT3 & M, const Tensor & T);
Tensor innerProduct(const Tensor & A, const Tensor & B);

VECX mat2Vec(const MAT3 & M);
MAT3 vec2Mat(const VECX & v);

std::ostream& operator<<(std::ostream& os, const Tensor& T);

Tensor rotateTensor(const Tensor & T, const MAT3 & R);
Tensor transformTensor(const Tensor & T, const MAT3 & M);

Tensor outerProduct(const MAT3 & M1, const MAT3 & M2);
#endif
  
