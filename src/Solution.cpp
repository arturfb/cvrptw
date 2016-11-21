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
   m_succ[k][v] = w;

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

   // Update route dist, time, size and load
   m_routeSize[k]--;
   m_routeDist[k] -= m_instance->getDistance(u,v);
   m_routeDist[k] -= m_instance->getDistance(v,w);
   m_routeDist[k] += m_instance->getDistance(u,w);
   m_customerTime[w] = m_instance->getService(w) + std::max((double)m_instance->getBtw(w), (m_customerTime[u] + m_instance->getDistance(u,w)));
   m_routeLoad[k] -= m_instance->getDemand(v);
   m_customerRoute[v] = -1;
}