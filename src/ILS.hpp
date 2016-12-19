#pragma once

#include <iostream>
#include <fstream>
#include <random>
#include "Solution.hpp"

class ILS {

public:
   ILS(Instance* inst);
   ~ILS();

   void run(const bool verbose, const unsigned timeLimit, const double pfactor, const unsigned ils_stagn, const unsigned vns_stagn);

private:
   Instance* m_instance;
   std::mt19937 m_randomGenerator;
   Solution tonn(); // Time-oriented nearest-neighbor
   Solution rtonn(Solution s); // Relaxed version of Time-oriented nearest-neighbor
   Solution twoExchange(Solution s, const unsigned k);
   Solution twoOpt(Solution s, const unsigned k);
   Solution moveCustomer(Solution s);
   Solution moveCustomerOpt(Solution s);
   Solution shift2(Solution s); // Two consecutive customers are transfered to the end of a different route
   Solution swap1(Solution s); // Swap two customers from different routes
   Solution perturb(Solution s, double factor); // Perturb a solution by removing (n * factor) customers and reinserting them in new routes 
   Solution vns(Solution s, const unsigned stagn);
};
