#pragma once

#include <iostream>
#include <fstream>
#include "Solution.hpp"

class VNS {

public:
   VNS(Instance* inst);
   ~VNS();
   // void run(const bool verbose, int vopt, double dopt, std::ofstream& results);
   void run(const bool verbose);

private:
   Instance* m_instance;
   Solution tonn(); // Time-oriented nearest-neighbor
   Solution rtonn(Solution s); // Relaxed version of Time-oriented nearest-neighbor
   Solution twoExchange(Solution s, const unsigned k);
   Solution twoOpt(Solution s, const unsigned k);
   Solution moveCustomer(Solution s);
   Solution moveCustomerOpt(Solution s);
   Solution shift2(Solution s); // Two consecutive customers are transfered to the end of a different route
   Solution swap1(Solution s); // Swap two customers from different routes
};
