#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include <chrono>
#include "ILS.hpp"

using namespace std;

ILS::ILS(Instance* inst) {
   m_instance = inst;
   m_randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}


ILS::~ILS(){
   // empty
}

void ILS::run(const bool verbose, const unsigned timeLimit, const double pfactor, const unsigned ils_stagn, const unsigned vns_stagn) {
	chrono::system_clock::time_point start = chrono::system_clock::now();
   
	// Initial solution
	Solution s = tonn();
	
   cout << s.getVehiclesUsed() << " " << s.getTotalDist() << " " << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-start).count()/1000 << " ";

	s = vns(s, vns_stagn);
   
   Solution best = s;
   unsigned stagnation = 0;
   unsigned ils_iters = 0;
   
   // ILS main loop
   while (stagnation < ils_stagn && chrono::duration_cast<chrono::seconds>(chrono::system_clock::now()-start).count() < timeLimit) {
      Solution ss = perturb(best, pfactor);
      ss = vns(ss, vns_stagn);
      
      // Acceptance criterion
      if (ss.getTotalDist() < best.getTotalDist()) {
         best = ss;
         stagnation = 0;
      }
      else {
         stagnation++;
      }
      ils_iters++;
   }
   s = best;


	if (verbose) {
	   s.print();
	}

   cout << s.getVehiclesUsed() << " " << s.getTotalDist() << " " << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-start).count()/1000 << " " << ils_iters << endl;
}

Solution ILS::vns(Solution s, const unsigned stagn) {
   // main body of the Variable Neighborhood Search
   // max_iter: maximum number of iterations without improvement
   unsigned max_iter = stagn;
   unsigned iter = 0;
   unsigned total_iter = 0;
   do {
         // n: current neighborhood ; n_max: number of neighborhoods
         unsigned n = 0, n_max = 2;

         bool improvement = false;
         do {
               Solution s1 = Solution(m_instance);
               Solution s2 = Solution(m_instance);

               // Shake; Generate a point s1 at random from the nth neighborhood of s
               if (n == 0)
               {
                  s1 = moveCustomer(s);
               }
               else
               {
                  s1 = s;

                  unsigned v = s.getVehiclesUsed();
                  unsigned k_init = rand()%v;
                  unsigned k = k_init;
                  do
                  {
                     if(s.getRouteSize(k) > 4)
                     {
                        s1 = twoExchange(s,k);
                        break;
                     }

                     k = (k+1)%v;
                  }
                  while( k != k_init );
               }

               s2 = s1;

               // Local search; Apply local search with s1 as initial solution. Obtain s2
               if (n == 0)
               {
                  double cost;
                  do
                  {
                     cost = s2.getTotalDist();
                     s2 = moveCustomerOpt(s2);
                  }
                  while(s2.getTotalDist() < cost);
                  
               }
               else
               {
                  unsigned v = s.getVehiclesUsed();

                  double cost;
                  do
                  {
                     cost = s2.getTotalDist();

                     for(unsigned k = 0; k < v; k++)
                     if(s2.getRouteSize(k) > 4)
                     {
                        s2 = twoOpt(s2,k);
                     }  
                  }
                  while(s2.getTotalDist() < cost);
               }

               // Acceptance; If f(s2) < f(s), then s = s2, n = 0. Else n++
               if (s2.getTotalDist() < s.getTotalDist())
               {
                  s = s2;
                  n = 0;
                  improvement = true;
               }
               else
               {
                  n++;
               }

         } while (n < n_max);

         if(!improvement) {
            iter++;
         }
         else {
            iter = 0;
         }
         total_iter++;
   } while (iter < max_iter);

   return s;
}

/*
	Implements the Time-Oriented Nearest Neighbor Heuristic (TONN)
	* starts at the first route
	* starting at the depot, searches for the nearest feasible customer to add to the current route
	* if there is no such customer, proceed to the next route
   See Algorithms for the Vehicle Routing and Scheduling Problem with Time Window Constraints, Solomon (1987)
   for a better understanding.
*/
Solution ILS::tonn() {
   Solution s(m_instance);

   double delta1 = 1.0; // Weight por physical distance
   double delta2 = 0.0; // Weight for waiting time
   double delta3 = 0.0; // Weight for urgency
   
   unsigned unroutedCount = m_instance->getCustomers();
   vector<char> unrouted(unroutedCount+1, 1); // 1 if customer is unrouted, 0 otherwise
   unrouted[0] = 0;

   unsigned currentRoute = 0;
   unsigned previousCustomer = 0;
   while (unroutedCount > 0) {
   	// initialize "nearest" with the maximum value for double variables
      double nearest = std::numeric_limits<double>::max();
      unsigned nearestIndex = 0;
      
      for (unsigned i = 1; i <= m_instance->getCustomers(); i++) {
         if (unrouted[i]) {
            double dij = m_instance->getDistance(previousCustomer, i); // These variables are named according Solomon (1987)
            double tij = max((double)m_instance->getBtw(i), (m_instance->getDistance(previousCustomer,i) + m_instance->getService(i))) - s.getCustomerTime(previousCustomer);
            double vij = m_instance->getEtw(i) - (s.getCustomerTime(previousCustomer) + m_instance->getDistance(previousCustomer,i));

            double cij = delta1 * dij + delta2 * tij + delta3 * vij; // Combined distance metric

            if (cij < nearest) {
               // Check feasibility (capacity and time-window)
               if (s.getRouteLoad(currentRoute) + m_instance->getDemand(i) <= m_instance->getCapacity()
                && s.getCustomerTime(previousCustomer) + m_instance->getDistance(previousCustomer, i) <= m_instance->getEtw(i)) {
                  	nearest = cij;
                  	nearestIndex = i;
               }
            }
         }
      }

      // Add nearest feasible customer to route
      if (nearestIndex > 0) {
         // cout << "Added " << nearestIndex << " to route " << currentRoute << endl;
         s.addToRoute(previousCustomer, nearestIndex, currentRoute);
         unroutedCount--;
         unrouted[nearestIndex] = 0;
         previousCustomer = nearestIndex;         
      }
      else { // Add new route if there isn't any feasible insertion
         // cout << "Created new route" << endl;
         currentRoute++;
         previousCustomer = 0;
      }
   }

   return s;
}

/*
	Implements the Relaxed version of the Time-Oriented Nearest Neighbor Heuristic (RTONN)
	* input: 		an incomplete route (that does not service all customers) generated by TONN
	* output: 		an infeasible route (breaking time-window constraints) that services all customers 
	* operation: 	repeatedly adds the overall nearest neighbor (all routes considered) to its respective route
*/
Solution ILS::rtonn(Solution s){
	
	unsigned unroutedCount = m_instance->getCustomers();
	vector<char> unrouted(unroutedCount+1, 1); // 1 if customer is unrouted, 0 otherwise
	unrouted[0] = 0;

	// lastCustomer[k] : the current last customer in the k-th route
	vector<unsigned> lastCustomer(s.getVehiclesUsed(),-1);

	// for each route k
	for(unsigned k = 0; k < s.getVehiclesUsed(); k++) {
		// for each customer in the k-th route
		unsigned cust = 0;
		for(unsigned i = 1; i < s.getRouteSize(k); i++) {
			cust = s.getSuccessor(cust,k);
			// mark 'cust' as routed and decrement the unrouted customers count
			unrouted[cust] = 0;
			unroutedCount--;

			if(i == s.getRouteSize(k)-1){
				lastCustomer[k] = cust;
			}
		}
	}

	// while there are unrouted customers
	while (unroutedCount > 0) {
   	  // initialize "nearest" with the maximum value for double variables
      double nearest = std::numeric_limits<double>::max();
      unsigned nearestIndex = -1;
      unsigned routeIndex = -1;

      // for each route k
      for(unsigned k = 0; k < s.getVehiclesUsed(); k++) {
      	// for each customer
      	for(unsigned i = 1; i <= m_instance->getCustomers(); i++){
      		// if client is unrouted and the current nearest neighbor among all routes
      		if (unrouted[i] && m_instance->getDistance(lastCustomer[k], i) < nearest) {
      			nearest = m_instance->getDistance(lastCustomer[k], i);
	            nearestIndex = i;
	            routeIndex = k;
      		}
      	}
      }

      // Add nearest feasible customer to route
      s.addToRoute(lastCustomer[routeIndex], nearestIndex, routeIndex);
      unroutedCount--;
      unrouted[nearestIndex] = 0;
      lastCustomer[routeIndex] = nearestIndex;
   }

   return s;
}

/*
   2-exchange neighborhood (returns a solution in the 2-exchange neighborhood of 's')
*/
Solution ILS::twoExchange(Solution s, const unsigned k) {

   // randomize 'initial_ci'
   unsigned aux = rand()%(s.getRouteSize(k));
   unsigned initial_ci = 0;
   for (unsigned i = 0; i < aux; i++) initial_ci = s.getSuccessor(initial_ci, k);

   unsigned ci  = initial_ci;
   do {
      unsigned cii = s.getSuccessor(ci, k);
      unsigned cj  = s.getSuccessor(cii, k);

      while (cj != ci) {

         unsigned cjj = s.getSuccessor(cj, k);

         // Check feasibility
         bool feasible = 1;
         
         // (1) Check new time at j;
         double arrival = max((double)m_instance->getBtw(cj), (s.getCustomerTime(ci) + m_instance->getDistance(ci,cj)));
         double departure = arrival + m_instance->getService(cj); 
         if (arrival > m_instance->getEtw(cj)) {
            feasible = 0;
         }
         
         // (2) then check from j-1 to i+1;
         unsigned currentCustomer = s.getPredecessor(cj, k);
         while (currentCustomer != ci) {
            arrival = max((double)m_instance->getBtw(currentCustomer), (departure + m_instance->getDistance(currentCustomer,s.getSuccessor(currentCustomer, k))));
            if (arrival > m_instance->getEtw(currentCustomer)) {
               feasible = 0;
            }
            departure = arrival + m_instance->getService(currentCustomer);
            currentCustomer = s.getPredecessor(currentCustomer, k);
         }

         // (3) check new time at j+1;
         arrival = max((double)m_instance->getBtw(cjj), (departure + m_instance->getDistance(cii,cjj)));
         if (arrival > m_instance->getEtw(cjj)) {
         	feasible = 0;
         }
         departure = arrival + m_instance->getService(cjj);
         
         // (4) and then the rest of the route.
         currentCustomer = s.getSuccessor(cjj,k);
         while (currentCustomer != ci) {
            arrival = max((double)m_instance->getBtw(currentCustomer), (departure + m_instance->getDistance(currentCustomer,s.getPredecessor(currentCustomer, k))));
            if (arrival > m_instance->getEtw(currentCustomer)) {
               feasible = 0;
            }
            departure = arrival + m_instance->getService(currentCustomer);
            currentCustomer = s.getSuccessor(currentCustomer, k);
         }

         // After all the feasibility checks, perform exchange
         if(feasible) 
         {
         	s.exchange(ci, cj, k);
         	return s;
         }

         cj = cjj;
      }

      ci = cii;
   } while (ci != initial_ci);

   return s;
}

/*
   2opt local search (First improvement, O(n^3))
*/
Solution ILS::twoOpt(Solution s, const unsigned k) {
   
   // randomize 'initial_ci'
   unsigned aux = rand()%(s.getRouteSize(k));
   unsigned initial_ci = 0;
   for (unsigned i = 0; i < aux; i++) initial_ci = s.getSuccessor(initial_ci, k);

   unsigned ci  = initial_ci;
   do {
      unsigned cii = s.getSuccessor(ci, k);
      unsigned cj  = s.getSuccessor(cii, k);

      while (cj != ci) {
         unsigned cjj = s.getSuccessor(cj, k);

         // Delta for the total route distance
         double delta = m_instance->getDistance(ci, cj)  + m_instance->getDistance(cii, cjj)
                      - m_instance->getDistance(ci, cii) - m_instance->getDistance(cj, cjj);


         // If the new route is shorter
         if (delta < 0 && abs(delta) > 1e-6) {

            // Check feasibility
            bool feasible = 1;
            
            // (1) Check new time at j;
            double arrival = max((double)m_instance->getBtw(cj), (s.getCustomerTime(ci) + m_instance->getDistance(ci,cj)));
            double departure = arrival + m_instance->getService(cj); 
            if (arrival > m_instance->getEtw(cj)) {
               feasible = 0;
            }
            
            // (2) then check from j-1 to i+1;
            unsigned currentCustomer = s.getPredecessor(cj, k);
            while (currentCustomer != ci) {
               arrival = max((double)m_instance->getBtw(currentCustomer), (departure + m_instance->getDistance(currentCustomer,s.getSuccessor(currentCustomer, k))));
               if (arrival > m_instance->getEtw(currentCustomer)) {
                  feasible = 0;
               }
               departure = arrival + m_instance->getService(currentCustomer);
               currentCustomer = s.getPredecessor(currentCustomer, k);
            }

            // (3) check new time at j+1;
            arrival = max((double)m_instance->getBtw(cjj), (departure + m_instance->getDistance(cii,cjj)));
            if (arrival > m_instance->getEtw(cjj)) {
            	feasible = 0;
            }
            departure = arrival + m_instance->getService(cjj);
            
            // (4) and then the rest of the route.
            currentCustomer = s.getSuccessor(cjj,k);
            while (currentCustomer != 0) {
               arrival = max((double)m_instance->getBtw(currentCustomer), (departure + m_instance->getDistance(currentCustomer,s.getPredecessor(currentCustomer, k))));
               if (arrival > m_instance->getEtw(currentCustomer)) {
                  feasible = 0;
               }
               departure = arrival + m_instance->getService(currentCustomer);
               currentCustomer = s.getSuccessor(currentCustomer, k);
            }

            // After all the feasibility checks, perform exchange
            if(feasible) {
            	s.exchange(ci, cj, k);
            	return s;
            }
         }

         cj = cjj;
      }

      ci = cii;
   } while (ci != initial_ci);

   return s;
}

/*
   2opt local search (O(n^2))
*/
// void ILS::twoOpt(Solution& s, const unsigned k) {
//    // This method implements an approach to evaluate feasibility in constant time.
//    // See "Sequential and parallel local search for the time-constrained traveling salesman problem",
//    // (Kindervater et al., 1993) for proofs and explanations.

//    bool restart = 0;
//    unsigned ci  = 0;
//    do {
//       unsigned cii = s.getSuccessor(ci, k);
//       unsigned cj  = s.getSuccessor(cii, k);


//       // Travel time for the path i+1 to j-1
//       double T = 0;
//       // Waiting time for the path i+1 to j-1
//       double W = 0;
//       // Maximum forward shift in time that wouldn't cause infeasibility
//       double S = 0;
//       // TODO: W and S initialization
//       // double S = m_instance->getEtw(cii) -;


      
//       while (cii != 0 && cj != 0) {
//          unsigned cjj = s.getSuccessor(cj, k);
//          unsigned pcj = s.getPredecessor(cj,k);
         
//          // Departure time for cj
//          double dcj = m_instance->getService(cj) + max((double)m_instance->getBtw(cj), (s.getCustomerTime(ci) + m_instance->getDistance(ci,cj)));
//          // Departure time for the predecessor of cj (before the addition of the edge to j)
//          double dpcj = m_instance->getService(pcj) + max((double)m_instance->getBtw(pcj), (s.getCustomerTime(ci) + m_instance->getDistance(ci,pcj)));

//          // Difference between new and old arrival time at j-1
//          double d = dcj + m_instance->getDistance(pcj, cj) - dpcj;

         
//          // Update variables
//          T += m_instance->getDistance(pcj, cj) + m_instance->getService(pcj);
//          W = max(W - d, 0.0);
//          S = min((double)m_instance->getEtw(cj) - dcj, S - d); 


//          // Departure time for cjj after edge exchange
//          double dcjj = m_instance->getService(cjj) + max((double)m_instance->getBtw(cjj), (dcj + T + W + m_instance->getDistance(cii,cjj)));
//          // Departure time for cjj before the edge exchange
//          double current_dcjj = m_instance->getService(cjj) + max((double)m_instance->getBtw(cjj), (s.getCustomerTime(cj) + m_instance->getDistance(cj,cjj)));


//          // Delta for the total route distance
//          double delta = m_instance->getDistance(ci, cj)  + m_instance->getDistance(cii, cjj)
//                       - m_instance->getDistance(ci, cii) - m_instance->getDistance(cj, cjj);


//          // If the new route is shorter and feasible
//          if (delta < 0 && /*S >= 0 &&*/ dcjj < current_dcjj) {
//             // Perform exchange
//             s.exchange(ci,cj,k);
//             restart = 1;
//             break;
//          }

//          cj = cjj;
//       }

//       ci = cii;
      
//       // Restart the search if an improvement was found
//       if (restart) {
//          // Restart
//       }
//    } while (ci != 0);
// }

Solution ILS::moveCustomer(Solution s){

	// shuffle the order in which customers are to be traversed in order to randomize the search
	vector<unsigned> shuffledCustomers(m_instance->getCustomers(),0);
	for(unsigned i = 0; i < m_instance->getCustomers(); i++){
		shuffledCustomers[i] = i+1;
	}
	random_shuffle(shuffledCustomers.begin(),shuffledCustomers.end());

	// for each customer 'i'
	for(unsigned i : shuffledCustomers){
		
		// get the route 'ki' to which customer 'i' belongs
		unsigned ki = s.getCustomerRoute(i);

		if(s.getRouteSize(ki) <= 1) continue;

		// for each other customer 'j' (including depot)
		for(unsigned j : shuffledCustomers){
			// get the route 'kj' to which customer 'j' belongs
			unsigned kj = s.getCustomerRoute(j);
			if(i != j && ki != kj){
				// check feasibility (capacity and time-window)
				if(s.getRouteLoad(kj) + m_instance->getDemand(i) <= m_instance->getCapacity()
					// && s.getCustomerTime(j) + m_instance->getDistance(j,i) <= m_instance->getEtw(i)){
               && s.checkInsertionFeasibility(j,i,kj)) { 

					s.remFromRoute(s.getPredecessor(i,ki),i,ki);
					s.addToRoute(j,i,kj);

					return s;
				}

			}
		}
	}

	return s;
}

Solution ILS::moveCustomerOpt(Solution s){

	// shuffle the order in which customers are to be traversed in order to randomize the search
	vector<unsigned> shuffledCustomers(m_instance->getCustomers(),0);
	for(unsigned i = 0; i < m_instance->getCustomers(); i++)
	{
		shuffledCustomers[i] = i+1;
	}
	random_shuffle(shuffledCustomers.begin(),shuffledCustomers.end());

	// for each customer 'i'
	for(unsigned i : shuffledCustomers)
	{
		// get the route 'ki' to which customer 'i' belongs
		unsigned ki = s.getCustomerRoute(i);

		// for each other customer 'j' (including depot)
		for(unsigned j : shuffledCustomers)
		{
			// get the route 'kj' to which customer 'j' belongs
			unsigned kj = s.getCustomerRoute(j);
			
			if(i != j && ki != kj)
			{
				// check feasibility (capacity and time-window)
				if(s.getRouteLoad(kj) + m_instance->getDemand(i) <= m_instance->getCapacity()
					// && s.getCustomerTime(j) + m_instance->getDistance(j,i) <= m_instance->getEtw(i)){
					&& s.checkInsertionFeasibility(j,i,kj)) { 

					// d1: the cost of removing customer 'i' from route 'ki'
					double d1 = - m_instance->getDistance(s.getPredecessor(i,ki),i) - m_instance->getDistance(i,s.getSuccessor(i,ki)) + m_instance->getDistance(s.getPredecessor(i,ki),s.getSuccessor(i,ki));

					// d2: the cost of adding customer 'i' to route 'kj'
					double d2 = - m_instance->getDistance(j,s.getSuccessor(j,kj)) + m_instance->getDistance(j,i) + m_instance->getDistance(i,s.getSuccessor(j,kj));

					// check if current neighbor has a better cost
					if( d1 + d2 < 0 )
					{
						s.remFromRoute(s.getPredecessor(i,ki),i,ki);
						s.addToRoute(j,i,kj);

						return s;
					}
				}

			}
		}
	}

	return s;
}

Solution ILS::shift2(Solution s) {
   // for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
   //    if (s.getRouteSize(k) >= 2) {
   //       for (unsigned i = 0; i < s.getRouteSize(k)-1; i++) {
   //          unsigned ci = s.getSuccessor(i, k);
   //          unsigned cii = s.getSuccessor(ci, k);
         
   //          for (unsigned kk = 0; kk < m_instance->getVehicles(); kk++) {
   //             if (k != kk && s.getRouteSize(kk) >= 1) {
   //                // Get first customer in candidate route
   //                unsigned customer = s.getSuccessor(0, kk);
   //                do {
   //                   // TODO: check delta first
   //                   // Check feasibility of insertion
   //                   if (s.getRouteLoad(kk) + m_instance->getDemand(ci) + m_instance->getDemand(cii) &&
   //                       ) {
   //                      /* code */
   //                   }

   //                } while (customer != 0);
   //             }
   //          }
   //       }
   //    }
   // }

   return s;
}

/*
   Swap neighborhood. Implements best improvement.
*/
Solution ILS::swap1(Solution s) {
   double bestDelta = 1;
   unsigned besti = 0, bestj = 0;

   for (unsigned i = 1; i < m_instance->getCustomers(); i++) {
      for (unsigned j = i + 1; j <= m_instance->getCustomers(); j++) {
         unsigned ri = s.getCustomerRoute(i);
         unsigned rj = s.getCustomerRoute(j);
         if (ri != rj) {
            unsigned pi = s.getPredecessor(i,ri);
            unsigned pj = s.getPredecessor(j,rj);
            // Check if the swap reduces the total distance
            double delta = - m_instance->getDistance(pi,i) - m_instance->getDistance(i,s.getSuccessor(i,ri))
                           + m_instance->getDistance(pi,j) + m_instance->getDistance(j,s.getSuccessor(i,ri))
                           - m_instance->getDistance(pj,j) - m_instance->getDistance(j,s.getSuccessor(j,rj))
                           + m_instance->getDistance(pj,i) + m_instance->getDistance(i,s.getSuccessor(j,rj));
            // cout << delta << endl;
            if (delta < 0) {
               // Check feasibility
               bool feasible = 0;
               unsigned Di = m_instance->getDemand(i);
               unsigned Dj = m_instance->getDemand(j);
               if (Di > Dj) {
                  if (s.getRouteLoad(rj) - Dj + Di <= m_instance->getCapacity()   &&
                      s.checkInsertionFeasibility(pi, j, ri)  &&
                      s.checkInsertionFeasibility(pj, i, rj)) {
                     
                     feasible = 1;  
                  }
               }
               else {
                  if (s.getRouteLoad(ri) - Di + Dj <= m_instance->getCapacity()   &&
                      s.checkInsertionFeasibility(pi, j, ri)  &&
                      s.checkInsertionFeasibility(pj, i, rj)) {

                     feasible = 1;
                  }   
               }

               if (feasible && delta <= bestDelta) {
                  bestDelta = delta;
                  besti = i;
                  bestj = j;
               }
            }
         }
      }
   }

   if (bestDelta <= 0) {
      unsigned ri = s.getCustomerRoute(besti);
      unsigned rj = s.getCustomerRoute(bestj);
      unsigned pi = s.getPredecessor(besti, ri);
      unsigned pj = s.getPredecessor(bestj, rj);
      s.remFromRoute(pi, besti, ri);
      s.remFromRoute(pj, bestj, rj);
      s.addToRoute(pi, bestj, ri);
      s.addToRoute(pj, besti, rj);
   }
         

   return s;
}

Solution ILS::perturb(Solution s, double factor) {
   unsigned previous = 0;
   unsigned routeIndex = s.getVehiclesUsed() - 1;
   uniform_int_distribution<unsigned>  distr(1, m_instance->getCustomers());
   vector<char> selected(m_instance->getCustomers() + 1, 0);

   for (unsigned i = 0; i < m_instance->getCustomers() * factor; i++) {
      // Select random customer
      unsigned randc = distr(m_randomGenerator);
      while (selected[randc]) { // Avoid reselecting a customer, as this can produce segmentation faults
         randc = distr(m_randomGenerator);
      }
      selected[randc] = 1;
      unsigned route = s.getCustomerRoute(randc);

      // Remove it from its route
      s.remFromRoute(s.getPredecessor(randc, route), randc, route);

      // Reinsert it into a new route or after "previous" if feasible
      if (previous == 0) {
         routeIndex++;
      }
      
      if (s.checkInsertionFeasibility(previous, randc, routeIndex)) {
         s.addToRoute(previous, randc, routeIndex);
         previous = randc;
      }
      else {
         routeIndex++;
         s.addToRoute(0, randc, routeIndex);
         previous = randc;
      }
   }

   return s;
}