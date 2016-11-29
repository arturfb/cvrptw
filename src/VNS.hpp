#pragma once


#include "Solution.hpp"

class VNS {

public:
   VNS(Instance* inst);
   ~VNS();
   void run(const bool verbose);
   
private:
   Instance* m_instance;
   Solution tonn(); // Time-oriented nearest-neighbor
   Solution rtonn(Solution s); // Relaxed version of Time-oriented nearest-neighbor
   void twoOpt(Solution& s, const unsigned k);
   Solution moveCustomerOpt(Solution s, bool& local_minimum);
};
