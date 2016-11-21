#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "VNS.hpp"
#include <limits> 

using namespace std;

VNS::VNS(Instance* inst) {
   m_instance = inst;	
}


VNS::~VNS(){
   // empty
}

Solution VNS::run(const bool verbose) {
   Solution s = tonn();
   
   /*
   unsigned n = 0, n_max = 2;
   do {
      // Shake; Generate a point s' at random from the nth neighborhood of s
      // LocalSearch; Apply LS with s' as initial solution. Obtain s''.
      // Acceptance; If this is a local optimum, then s = s'', n = 0. Else n++.
   } while (n < n_max);
   */

   return s;
}

/*
	Implements the Time-Oriented Nearest Neighbor Heuristic (TONN)
	* starts at the first route
	* starting at the depot, searches for the nearest feasible customer to add to the current route
	* if there is no such customer, proceed to the next route

	current issues:
	1. TONN is unable to build a route, for a large number of Cordeau's and Solomon's instances.
		* this happens because the number of vehicles is exhausted before all customers are serviced
		* the percentage of serviced customers ranges from 30% to 76% for "c" instances ("c101","c102", etc.) of the Cordeau dataset
		* proposed solution: to work in the infeasibility. if the number of vehicles is exhausted, restart TONN relaxing the time windows constraints

*/
Solution VNS::tonn() {
   Solution s(m_instance);
   
   unsigned unroutedCount = m_instance->getCustomers();
   vector<char> unrouted(unroutedCount+1, 1); // 1 if customer is unrouted, 0 otherwise
   unrouted[0] = 0;

   unsigned currentRoute = 0;
   unsigned previousCustomer = 0;
   while (unroutedCount > 0) {
   	  // initialize "nearest" with the maximum value for double variables
      double nearest = std::numeric_limits<double>::max();
      unsigned nearestIndex = -1;
      
      for (unsigned i = 1; i <= m_instance->getCustomers(); i++) {
         if (unrouted[i] && m_instance->getDistance(previousCustomer, i) < nearest) {
            // Check feasibility
            if (s.getRouteLoad(currentRoute) + m_instance->getDemand(i) <= m_instance->getCapacity()
             && s.getCustomerTime(previousCustomer) + m_instance->getDistance(previousCustomer, i) + m_instance->getService(i) <= m_instance->getEtw(i)) {
               
               nearest = m_instance->getDistance(previousCustomer, i);
               nearestIndex = i;
            }
         }
      }

      // Add nearest feasible customer to route
      if (nearestIndex != (unsigned)(-1)) {
         s.addToRoute(previousCustomer, nearestIndex, currentRoute);
         unroutedCount--;
         unrouted[nearestIndex] = 0;
         previousCustomer = nearestIndex;
      }
      else { // Add new route if there isn't any feasible insertion
         currentRoute++;
         previousCustomer = 0;

         if(currentRoute >= m_instance->getVehicles()){
         	cout << "ERROR (tonn): Ran out of vehicles. Percentage of clients served: " << (double)100*(m_instance->getCustomers() - unroutedCount)/m_instance->getCustomers() << "\%" << endl;
         	exit(0);
         }
      }
   }

   return s;
}

Solution VNS::twoOpt(Solution s, const unsigned k) {
   /*unsigned ci  = 0;
   do {
      unsigned cii = s->getSuccessor(ci, k);
      unsigned cj  = s->getSuccessor(cii, k);
      while (cii != 0 && cj != 0) {
         unsigned cjj = s->getSuccessor(cj, k);

         double delta = m_instance->getDistance(ci, cj)  + m_instance->getDistance(cii, cjj)
                      - m_instance->getDistance(ci, cii) - m_instance->getDistance(cj, cjj);

         // If the new route is shorter
         if (delta < 0) {
            // Check feasibility
         }

         cj = cjj;
      }

      ci = cii;
   } while (ci != 0);*/

   return s;
}