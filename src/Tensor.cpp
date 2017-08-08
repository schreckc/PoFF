#include "Tensor.hpp"
#include "error.hpp"

Tensor::Tensor(FLOAT d): dim(d) {
  t =  std::vector<FLOAT>(pow(d,4));
  for (uint i = 0; i < t.size(); ++i) {
    t[i] = 0;
  }
  for (uint i = 0; i < dim; ++i) {
    for (uint j = 0; j < dim; ++j) {
      (*this)(i, j, i, j) = 1;
      //(*this)(i, j, j, i) = 1;
    }
    //    (*this)(i, i, i, i) = 1;
  }
}

  

FLOAT& Tensor::operator() (int i, int j, int k, int l) {
  int ind = pow(dim,3)*i+pow(dim,2)*k+dim*j+l;
  return t[ind];
}

const FLOAT& Tensor::operator()(int i, int j, int k, int l) const {
  int ind = pow(dim,3)*i+pow(dim, 2)*k+dim*j+l;
  return t[ind];
}


MATX tensor2Mat(const Tensor & T) {
  MATX M(2*T.dim, 2*T.dim);
  
  for (uint i = 0; i < T.dim; ++i) {
    for (uint j = 0; j < T.dim; ++j) {
      M(i, j) = T(i, i, j, j);
      M(i+3, j) = T((i+1)%3, (i+2)%3, j, j);
      M(i, j+3) = T(i, i, (j+1)%3, (j+2)%3);
      M(i+3, j+3) = T((i+1)%3, (i+2)%3, (j+1)%3, (j+2)%3);
    }
  }
  TEST(false);
  //need debbugging
  return M;
}

Tensor mat2Tensor(const MATX & M) {
  Tensor T;
  TEST(M.cols() == M.rows());
  T = Tensor(M.cols()/2);
 
  for (uint i = 0; i < T.dim; ++i) {
    for (uint j = 0; j < T.dim; ++j) {
      T(i, i, j, j) = M(i, j);
      
      T((i+1)%3, (i+2)%3, (j+1)%3, (j+2)%3) = M(i+3, j+3);
      T((i+2)%3, (i+1)%3, (j+1)%3, (j+2)%3) = M(i+3, j+3);
      T((i+1)%3, (i+2)%3, (j+2)%3, (j+1)%3) = M(i+3, j+3);
      T((i+2)%3, (i+1)%3, (j+2)%3, (j+1)%3) = M(i+3, j+3);

      T((i+1)%3, (i+2)%3, j, j) = M(i+3, j);
      T((i+2)%3, (i+1)%3, j, j) = M(i+3, j);
      T(j, j, (i+1)%3, (i+2)%3) = M(i+3, j);
      T(j, j, (i+2)%3, (i+1)%3) = M(i+3, j);
    }
  }
  TEST(false);
  //need debbugging
  return T;
}


MATX tensor2MatOrtho(const Tensor & T) {
  MATX M(2*T.dim, 2*T.dim);
  
  for (uint i = 0; i < T.dim; ++i) {
    for (uint j = 0; j < T.dim; ++j) {
      M(i, j) = T(i, i, j, j);
      M(i+3, j) = 0;
      M(i, j+3) = 0;
      M(i+3, j+3) = 0;
    }
    M(i+3, i+3) = 2*T((i+1)%3, (i+2)%3, (i+1)%3, (i+2)%3);// + T((i+1)%3, (i+2)%3, (i+2)%3, (i+1)%3) ;
  }
  return M;
}

Tensor mat2TensorOrtho(const MATX & M) {
  Tensor T;
  TEST(M.cols() == M.rows());
  T = Tensor(M.cols()/2);
 
  for (uint i = 0; i < T.dim; ++i) {
    for (uint j = 0; j < T.dim; ++j) {
      T(i, i, j, j) = M(i, j);
    }
    T((i+1)%3, (i+2)%3, (i+1)%3, (i+2)%3) = 0.5*M(i+3, i+3);
    T((i+2)%3, (i+1)%3, (i+1)%3, (i+2)%3) =  0.5*M(i+3, i+3);
    T((i+2)%3, (i+1)%3, (i+2)%3, (i+1)%3) =  0.5*M(i+3, i+3);
    T((i+1)%3, (i+2)%3, (i+2)%3, (i+1)%3) =  0.5*M(i+3, i+3);
    // T((i+1)%3, (i+2)%3, (i+2)%3, (i+1)%3) = 0.5*M(i+3, i+3);
  }
  return T;
}


MAT3 innerProduct(const Tensor & T, const MAT3 & M) {
  MAT3 P;
  uint d = T.dim;
  for (uint i = 0; i < d; ++i) {
    for (uint j = 0; j < d; ++j) {

      P(i, j) = 0;
      for (uint k = 0; k < d; ++k) {
	for (uint l = 0; l < d; ++l) {
	  P(i, j) += T(i, j, k, l)*M(k, l);
	}
      }

    }
  }
  return P;
}
  
Tensor innerProduct(const Tensor & A, const Tensor & B) {
  uint d = A.dim;
  TEST(B.dim == d);
  Tensor T(d);
  for (uint i = 0; i < d; ++i) {
    for (uint j = 0; j < d; ++j) {
      for (uint k = 0; k < d; ++k) {
	for (uint l = 0; l < d; ++l) {

	  T(i, j, k, l) = 0;
	  for (uint m = 0; m < d; ++m) {
	    for (uint n = 0; n < d; ++n) {
	      T(i, j, k, l) += A(i, j, m, n)*B(m, n, k, l);
	    }
	  }

	}
      }
    }
  }
  return T;
}


VECX mat2Vec(const MAT3 & M) {
  VECX v(6);
  for (uint i = 0; i < 3; ++i) {
    v(i) = M(i, i);
    v(i+3) = M((i+1)%3, (i+2)%3);
  }
  return v;
}
  
MAT3 vec2Mat(const VECX & v) {
  MAT3 M;
  for (uint i = 0; i < 3; ++i) {
    M(i, i) = v(i);
    M((i+1)%3, (i+2)%3) = v(i+3);
    M((i+2)%3, (i+1)%3) = v(i+3);
  }
  return M;
}


std::ostream& operator<<(std::ostream& os, const Tensor& T) {
  uint dim = 3;
  os<<"\n---------------\n";
  for (uint i = 0; i < dim; ++i) {
     for (uint k = 0; k < dim; ++k) {
    os << "| ";
       for (uint j = 0; j < dim; ++j) {
     	for (uint l = 0; l < dim; ++l) {
	  if (fabs(T(i, j, k, l)) > 1e-15) {
	    os<<T(i, j, k, l)<<" ";
	  } else {
	    os<<"0 ";
	  }
     	}
     	os<<"| ";
       }
       os <<"\n";
     }
    os<<"\n---------------\n";
  }
  return os;
}


Tensor rotateTensor(const Tensor & T, const MAT3 & R) {
  uint d = T.dim;
  Tensor rotT(d);
  for (uint i = 0; i < d; ++i) {
    for (uint j = 0; j < d; ++j) {
      for (uint k = 0; k < d; ++k) {
	for (uint l = 0; l < d; ++l) {
	  FLOAT rotT_ijkl = 0;
	  for (uint m = 0; m < d; ++m) {
	    for (uint n = 0; n < d; ++n) {
	      for (uint o = 0; o < d; ++o) {
		for (uint p = 0; p < d; ++p) {
		  rotT_ijkl += R(i, m)*R(j, n)*R(k, o)*R(l, p)*T(m, n, o, p);
		}
	      }
	    }
	  }
	  if (std::fabs(rotT_ijkl) > 10e-12) {
	    rotT(i, j, k, l) = rotT_ijkl;
	  } else {
	    rotT(i, j, k, l) = 0;
	  }
	}
      }
    }
  }
  return rotT;
}
