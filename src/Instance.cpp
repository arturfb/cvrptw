#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <stdlib.h>
#include <cmath>
#include "Instance.hpp"

using namespace std;

Instance::Instance() {
   // empty
}


Instance::~Instance(){
   // empty
}

void Instance::loadFromFile(const string file) {
   fstream fs(file, ios::in);

   if (!fs.is_open()) {
	  cerr << "\nFile could not be opened.\n" << endl;
	  exit(1);
   }

   string name(file);

   // Cordeau
   if (name.find("cordeau") != string::npos) {
      fs >> m_type >> m_vehicles >> m_customers >> m_depots;
      m_vehicles = m_customers; // Overwrite value as our approach also minimizes the number of used vehicles

      m_D.resize(m_depots);
      m_Q.resize(m_depots);
      for (unsigned i = 0; i < m_depots; i++) {
         fs >> m_D[i] >> m_Q[i];
      }
      m_capacity = m_Q[0];

      m_x.resize(m_customers+1);
      m_y.resize(m_customers+1);
      m_d.resize(m_customers+1);
      m_q.resize(m_customers+1);
      m_f.resize(m_customers+1);
      m_a.resize(m_customers+1);
      m_btw.resize(m_customers+1);
      m_etw.resize(m_customers+1);
      m_list.resize(m_customers+1);
      for (unsigned i = 0; i <= m_customers; i++) {
         string s;
         fs >> s; // Customer index, not useful
         fs >> s;
         m_x[i] = stod(s);
         fs >> s;
         m_y[i] = stod(s);
         fs >> s;
         m_d[i] = stoul(s);
         fs >> s;
         m_q[i] = stoul(s);
         fs >> s;
         m_f[i] = stoul(s);
         fs >> s;
         m_a[i] = stoul(s);
         m_list[i].resize(m_a[i]);
         for (unsigned j = 0; j < m_a[i]; j++) {
            fs >> s;
            m_list[i][j] = stoul(s);
         }
         fs >> s;
         m_btw[i] = stoul(s);
         fs >> s;
         m_etw[i] = stoul(s);
      }

      // Customer n+1 is the depot
      m_x.push_back(m_x[0]);
      m_y.push_back(m_y[0]);
      m_q.push_back(m_q[0]);
      m_d.push_back(m_d[0]);
      m_btw.push_back(m_btw[0]);
      m_etw.push_back(m_etw[0]);
      m_f.push_back(m_f[0]);
      m_a.push_back(m_a[0]);
      m_list.push_back(m_list[0]);
   }

   // Solomon/Homberger
   else if (name.find("solomon") != string::npos || name.find("homberger") != string::npos) {
      fs.ignore(256,'Y'); // Skip header
      string s;
      
      fs >> s;
      m_customers = m_vehicles = stoul(s);
      fs >> s;
      m_capacity = stoul(s);

      fs.ignore(256,'0'); // Skip header, read first customer number
      while (!fs.eof()) {
         fs >> s;
         m_x.push_back(stod(s));
         fs >> s;
         m_y.push_back(stod(s));
         fs >> s;
         m_q.push_back(stoul(s));
         fs >> s;
         m_btw.push_back(stoul(s));
         fs >> s;
         m_etw.push_back(stoul(s));
         fs >> s;
         m_d.push_back(stoul(s));

         fs >> s; // Read next customer number
      }

      // Customer n+1 is the depot
      m_x.push_back(m_x[0]);
      m_y.push_back(m_y[0]);
      m_q.push_back(m_q[0]);
      m_d.push_back(m_d[0]);
      m_btw.push_back(m_btw[0]);
      m_etw.push_back(m_etw[0]);
   }
   else {
      cerr << endl << "Unknown instance format!" << endl;
      exit(1);
   }

   // Calculate and store distance between customers
   m_distance.resize(m_customers + 2);
   for (unsigned i = 0; i <= m_customers + 1; i++) {
      m_distance[i].resize(m_customers + 2);
      for (unsigned j = 0; j <= m_customers + 1; j++) {
         double dx = m_x[j]-m_x[i];
         double dy = m_y[j]-m_y[i];
         m_distance[i][j] = sqrt((dx*dx)+(dy*dy));
      }
   }

   fs.close();
   // cout << "Instance: " << name << endl;
}

void Instance::print() {
   cout << "Customers: " << m_customers << endl;
   cout << "Vehicles: " << m_vehicles << endl;
   cout << "Capacity: " << m_capacity << endl;
   cout << " Demands:";
   for (unsigned i = 0; i <= m_customers + 1; i++) {
      cout << " " << m_q[i];
   }
   cout << endl << "Service durations:";
   for (unsigned i = 0; i <= m_customers + 1; i++) {
      cout << " " << m_d[i];
   }
   cout << endl << "Time windows:";
   for (unsigned i = 0; i <= m_customers + 1; i++) {
      cout << " (" << m_btw[i] << "," << m_etw[i] << ")";
   }
   cout << endl << "Coordinates:";
   for (unsigned i = 0; i <= m_customers + 1; i++) {
      cout << " (" << m_x[i] << "," << m_y[i] << ")";
   }
   cout << endl;
}

const unsigned Instance::getVehicles() {
   return m_vehicles;
}

const unsigned Instance::getCustomers() {
   return m_customers;
}

const unsigned Instance::getCapacity() {
   return m_capacity;
}

const unsigned Instance::getDemand(unsigned c) {
   return m_q[c];
}

const unsigned Instance::getBtw(unsigned c) {
   return m_btw[c];
}

const unsigned Instance::getEtw(unsigned c) {
   return m_etw[c];
}

const unsigned Instance::getService(unsigned c) {
   return m_d[c];
}

const double Instance::getDistance(unsigned u, unsigned v) {
   return m_distance[u][v];
}