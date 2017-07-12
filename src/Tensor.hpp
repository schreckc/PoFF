#ifndef TENSOR_HPP
#define TENSOR_HPP

#include "mpm_conf.hpp"
#include "Eigen/Dense"
#include "Eigen/Core"

class Tensor {
  uint dim;
  std::vector<FLOAT> t;

public: 
  Tensor(FLOAT d = 3);
  FLOAT& operator() (int i, int j, int k, int l);
  const FLOAT& operator()(int i, int j, int k, int l) const;

  // Tensor inverse() const;
  // Tensor inverseOrtho() const;

  friend MATX tensor2Mat(const Tensor & T);
  friend Tensor mat2Tensor(const MATX & M);

  friend MATX tensor2MatOrtho(const Tensor & T);
  friend Tensor mat2TensorOrtho(const MATX & M);

  friend MAT3 innerProduct(const Tensor & T, const MAT3 & M);
  friend Tensor innerProduct(const Tensor & A, const Tensor & B);

  friend std::ostream& operator<<(std::ostream& os, const Tensor& T);
};

MATX tensor2Mat(const Tensor & T);
Tensor mat2Tensor(const MATX & M);

MATX tensor2MatOrtho(const Tensor & T);
Tensor mat2TensorOrtho(const MATX & M);

MAT3 innerProduct(const Tensor & T, const MAT3 & M);
Tensor innerProduct(const Tensor & A, const Tensor & B);

VECX mat2Vec(const MAT3 & M);
MAT3 vec2Mat(const VECX & v);

std::ostream& operator<<(std::ostream& os, const Tensor& T);

#endif
  
