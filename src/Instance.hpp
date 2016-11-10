#pragma once

#include <fstream>
#include <vector>
#include <string>

class Instance {

public:
   Instance();
   ~Instance();
   
   void loadFromFile(const std::string file);
   void print();
   
   const unsigned getVehicles();
   const unsigned getCustomers();
   const unsigned getCapacity();
   const unsigned getDemand(unsigned c);
   const unsigned getBtw(unsigned c);
   const unsigned getEtw(unsigned c);
   const unsigned getService(unsigned c);
   const double getDistance(unsigned u, unsigned v);
   
private:
   unsigned m_type;      // Instance type according to Cordeau
   unsigned m_vehicles;  // Number of vehicles
   unsigned m_customers; // Number of clients
   unsigned m_depots;    // t (Cordeau only, see ../instances/cordeau/README.txt)
   unsigned m_capacity;  // Vehicle capacity (Solomon and Homberger)
   std::vector<std::vector<double>> m_distance; // Distances between depot/clients

   // For each t (Cordeau only)
   std::vector<unsigned> m_D; // Maximum duration of a route
   std::vector<unsigned> m_Q; // maximum load of a vehicle
   
   // For each client
   std::vector<double> m_x;     // X coordinate
   std::vector<double> m_y;     // Y coordinate
   std::vector<unsigned> m_d;   // Service duration
   std::vector<unsigned> m_q;   // Demand
   std::vector<unsigned> m_f;   // Frequency of visit
   std::vector<unsigned> m_a;   // Number of possible visit combinations
   std::vector<unsigned> m_btw; // Beginning of time window
   std::vector<unsigned> m_etw; // End of time window
   std::vector<std::vector<unsigned>> m_list; // List of all possible visit combinations
 };
