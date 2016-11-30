#pragma once

#include <fstream>
#include <vector>
#include <string>
#include "Instance.hpp"

class Solution {

public:
   Solution(Instance* inst);
   ~Solution();
   
   unsigned getSuccessor(const unsigned c, const unsigned k); // Returns the successor of client c in route k
   unsigned getPredecessor(const unsigned c, const unsigned k); // Returns the predecessor of client c in route k
   double getTotalDist();
   double getRouteDist(const unsigned k);
   double getRouteTime(const unsigned k);
   unsigned getRouteSize(const unsigned k);
   unsigned getRouteLoad(const unsigned k);
   unsigned getCustomerRoute(const unsigned c);
   double getCustomerTime(const unsigned c);
   unsigned getVehiclesUsed();

   void addToRoute(const unsigned u, const unsigned v, const unsigned k);  // Add v after u in route k
   void remFromRoute(const unsigned u, const unsigned v, const unsigned k);// Remove v (after u) from route k
   void exchange(const unsigned u, const unsigned v, const unsigned k); // Exchange edges (u,succ[u]) and (v,succ[v]) for (u,v) and (succ[u],succ[v]) and reverse the path between them
   
   void print(); // Print each route

private:
   Instance* m_instance;
   std::vector<std::vector<unsigned>> m_succ; // Successor of each customer
   std::vector<std::vector<unsigned>> m_pred; // Predecessor of each customer
   std::vector<double> m_routeDist;       // Total distance of a route
   std::vector<double> m_routeTime;       // Total time taken by a route ** Not used **
   std::vector<unsigned> m_routeSize;     // Number of clients in a route
   std::vector<unsigned> m_routeLoad;     // Load of the vehicle attending the route
   std::vector<double> m_customerTime;    // Time at which a customer is attended
   std::vector<int> m_customerRoute; // Index of the route the customer is in
 };
