#pragma once

#include <iostream>
#include <fstream>
#include "Solution.hpp"

class VNS {

public:
   VNS(Instance* inst);
   ~VNS();
   void run(const bool verbose, int vopt, double dopt, std::ofstream& results);
   
private:
   Instance* m_instance;
   Solution tonn(); // Time-oriented nearest-neighbor
   Solution rtonn(Solution s); // Relaxed version of Time-oriented nearest-neighbor
   Solution twoExchange(Solution s, const unsigned k);
   Solution twoOpt(Solution s, const unsigned k);
   Solution moveCustomer(Solution s);
   Solution moveCustomerOpt(Solution s);
};
