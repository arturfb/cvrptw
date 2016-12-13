#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <stdlib.h>
#include <cmath>
#include "Solution.hpp"

using namespace std;

Solution::Solution(Instance* inst) {
   m_instance = inst;
   m_succ.resize(m_instance->getVehicles());
   m_pred.resize(m_instance->getVehicles());
   for (unsigned i = 0; i < m_succ.size(); i++) {
      m_succ[i].resize(m_instance->getCustomers()+1);
      m_pred[i].resize(m_instance->getCustomers()+1);
   }
   m_routeDist.resize(m_instance->getVehicles());
   m_routeTime.resize(m_instance->getVehicles());
   m_routeSize.resize(m_instance->getVehicles());
   m_routeLoad.resize(m_instance->getVehicles());
   m_customerTime.resize(m_instance->getCustomers()+1);
   m_customerRoute.resize(m_instance->getCustomers()+1, -1);
}


Solution::~Solution(){
   // empty
}

double Solution::getTotalDist() {
   // double d = 0;
   // for (unsigned i = 0; i < m_succ.size(); i++) {
   //    unsigned c = 0;
   //    do {
   //       d += m_instance->getDistance(c, m_succ[i][c]);
   //       c = m_succ[i][c];
   //    } while (c != 0);
   // }

   double d = 0;
   for (unsigned i = 0; i < m_routeDist.size(); i++) {
      d += m_routeDist[i];
   }

   return d;
}

unsigned Solution::getSuccessor(const unsigned c, const unsigned k) {
   return m_succ[k][c];
}

unsigned Solution::getPredecessor(const unsigned c, const unsigned k) {
   return m_pred[k][c];
}

double Solution::getRouteDist(const unsigned k) {
   return m_routeDist[k];
}

double Solution::getRouteTime(const unsigned k) {
   return m_routeTime[k];
}

unsigned Solution::getRouteSize(const unsigned k) {
   return m_routeSize[k];
}

unsigned Solution::getRouteLoad(const unsigned k) {
   return m_routeLoad[k];
}

unsigned Solution::getCustomerRoute(const unsigned c) {
   return m_customerRoute[c];
}

double Solution::getCustomerTime(const unsigned c) {
   return m_customerTime[c];
}

unsigned Solution::getVehiclesUsed() {
   unsigned v = 0;
   for (unsigned i = 0; i < m_succ.size(); i++) {
      if (m_succ[i][0] != 0) {
         v++;
      }
   }
   return v;
}

void Solution::addToRoute(const unsigned u, const unsigned v, const unsigned k) {
   unsigned w = m_succ[k][u];
   m_succ[k][u] = v;
   m_pred[k][v] = u;
   m_succ[k][v] = w;
   m_pred[k][w] = v;

   // Update route distance, time, size and load
   m_routeSize[k]++;
   m_routeDist[k] -= m_instance->getDistance(u,w);
   m_routeDist[k] += m_instance->getDistance(u,v);
   m_routeDist[k] += m_instance->getDistance(v,w);
   m_customerTime[v] = m_instance->getService(v) + std::max((double)m_instance->getBtw(v), (m_customerTime[u] + m_instance->getDistance(u,v)));
   m_routeLoad[k] += m_instance->getDemand(v);
   m_customerRoute[v] = k;
}

void Solution::remFromRoute(const unsigned u, const unsigned v, const unsigned k) {

   unsigned w = m_succ[k][v];

   m_succ[k][u] = w;
   m_pred[k][w] = u;

   m_succ[k][v] = -1;
   m_pred[k][v] = -1;

   // Update route dist, time, size and load
   m_routeSize[k]--;

   m_routeDist[k] -= m_instance->getDistance(u,v);
   m_routeDist[k] -= m_instance->getDistance(v,w);
   m_routeDist[k] += m_instance->getDistance(u,w);

   m_customerTime[w] = m_instance->getService(w) + std::max((double)m_instance->getBtw(w), (m_customerTime[u] + m_instance->getDistance(u,w)));
   m_routeLoad[k] -= m_instance->getDemand(v);
   m_customerRoute[v] = -1;
}

void Solution::checkEmptyRoute(const unsigned k) {

	// If the route size dropped to 0, we update the number of vehicles and apply shifts to:
    // m_succ[k], m_pred[k], m_routeDist[k], m_routeTime[k], m_routeSize[k], m_routeLoad[k]

   if (k < m_succ.size()-1 && m_routeSize[k] == 0)
   	{
   		unsigned v = getVehiclesUsed();
		
		for (unsigned k1 = k+1; k1 < m_succ.size(); k1++) 
		{
			m_succ[k1-1] = m_succ[k1];
			m_pred[k1-1] = m_pred[k1];

			m_routeDist[k1-1] = m_routeDist[k1];
			m_routeTime[k1-1] = m_routeTime[k1];
			m_routeSize[k1-1] = m_routeSize[k1];
			m_routeLoad[k1-1] = m_routeLoad[k1];
		}

		if ( v > 1 )
		{
			for(unsigned i = 0; i < m_instance->getCustomers(); i++)
			{
				m_succ[v-2][i] = m_pred[v-2][i] = 0;
			}
			
			m_routeDist[v-2] = m_routeTime[v-2] = m_routeSize[v-2] = m_routeLoad[v-2] = 0;
		}

		for (unsigned k1 = 0; k1 < v-1; k1++)
		{
			unsigned cust = 0;
			for(unsigned i = 0; i < m_routeSize[k1]; i++)
			{
				cust = m_succ[k1][cust];
				if(cust == 0) break;
				m_customerRoute[cust] = k1;
			}
		}
   }
}

void Solution::exchange(const unsigned u, const unsigned v, const unsigned k) {
   unsigned su = m_succ[k][u];
   unsigned sv = m_succ[k][v];
   
   // Add edge (u,v)
   m_succ[k][u] = v;
   m_routeDist[k] -= m_instance->getDistance(u, su);
   m_routeDist[k] += m_instance->getDistance(u, v);
   m_customerTime[v] = m_instance->getService(v) + max((double)m_instance->getBtw(v), (m_customerTime[u] + m_instance->getDistance(u,v)));
   
   double currentTime = m_customerTime[v];
   unsigned currentCustomer = m_pred[k][v];
   unsigned pred = m_pred[k][currentCustomer];
   unsigned succ = m_succ[k][currentCustomer];

   m_pred[k][v] = u;
   m_succ[k][v] = currentCustomer;
   
   // Reverse the path from su to v
   while (currentCustomer != su) {
      m_pred[k][currentCustomer] = succ;
      m_succ[k][currentCustomer] = pred;
      currentTime = m_instance->getService(currentCustomer) + max((double)m_instance->getBtw(currentCustomer), (m_customerTime[m_pred[k][currentCustomer]] + m_instance->getDistance(m_pred[k][currentCustomer],currentCustomer)));
      m_customerTime[currentCustomer] = currentTime;

      currentCustomer = m_succ[k][currentCustomer];
      succ = m_succ[k][currentCustomer];
      pred = m_pred[k][currentCustomer];
   }
   
   // Add edge (su,sv)
   m_pred[k][su] = succ;
   m_succ[k][su] = sv;
   m_routeDist[k] -= m_instance->getDistance(v,sv);
   m_routeDist[k] += m_instance->getDistance(su,sv);
   m_customerTime[su] = m_instance->getService(su) + max((double)m_instance->getBtw(su), (m_customerTime[m_pred[k][su]] + m_instance->getDistance(m_pred[k][su],su)));

   // Update sv and the rest of the route
   m_pred[k][sv] = su;
   if (sv != 0) {
      m_customerTime[sv] = m_instance->getService(sv) + max((double)m_instance->getBtw(sv), (m_customerTime[m_pred[k][sv]] + m_instance->getDistance(m_pred[k][sv],sv)));

      // Check the time of attendance for the rest of the customers in the route
      currentCustomer = m_succ[k][sv];
      while (currentCustomer != 0) {
         m_customerTime[currentCustomer] = m_instance->getService(currentCustomer) + max((double)m_instance->getBtw(currentCustomer), (m_customerTime[m_pred[k][currentCustomer]] + m_instance->getDistance(m_pred[k][currentCustomer],currentCustomer)));
         currentCustomer = m_succ[k][currentCustomer];
      }
   }
}

bool Solution::checkInsertionFeasibility(const unsigned u, const unsigned v, const unsigned k) {
   // (1) Check arrival time at v;
   double arrival = max((double)m_instance->getBtw(v), (getCustomerTime(u) + m_instance->getDistance(u,v)));
   double departure = arrival + m_instance->getService(v); 
   if (arrival > m_instance->getEtw(v)) {
      return 0;
   }

   // (2) Check arrival time at current successor of u;
   unsigned currentCustomer = getSuccessor(u,k);
   if (currentCustomer == 0) { // If u is the only customer in the route
      return 1;
   }
   arrival = max((double)m_instance->getBtw(currentCustomer), (departure + m_instance->getDistance(v,currentCustomer)));
   if (arrival > m_instance->getEtw(currentCustomer)) {
      return 0;
   }
   departure = arrival + m_instance->getService(currentCustomer);

   // (3) Check the rest of the route.
   currentCustomer = getSuccessor(currentCustomer,k);
   while (currentCustomer != 0) {
      arrival = max((double)m_instance->getBtw(currentCustomer), (departure + m_instance->getDistance(currentCustomer,getPredecessor(currentCustomer, k))));
      if (arrival > m_instance->getEtw(currentCustomer)) {
         return 0;
      }
      departure = arrival + m_instance->getService(currentCustomer);
      currentCustomer = getSuccessor(currentCustomer, k);
   }

   return 1;
}

void Solution::print() {
   unsigned routeCount = 0;
   for (unsigned k = 0; k < m_succ.size(); k++) {
      if (m_succ[k][0] != 0) {
         cout << "Route " << routeCount << ": ";
         unsigned c = 0;
         do {
            cout << c << " ";
            c = m_succ[k][c];
         } while (c != 0);
         cout << endl;
         routeCount++;
      }
   }
}