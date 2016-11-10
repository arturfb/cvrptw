#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include <chrono>
#include "Model.hpp"

using namespace std;

const unsigned K = 10000;

Model::Model(IloEnv &env, Instance* inst) : m_instance(inst),
                           m_env(env),
                           m_model(m_env),
                           m_cplex(m_model),
                           m_obj(m_env),
                           m_x(m_env),
                           m_s(m_env) {
   
   char name[30];

   // Prepare variables x and s
   m_x = IloArray<NumVarMatrix>(m_env, m_instance->getCustomers()+2);
   for (unsigned i = 0; i < m_instance->getCustomers()+2; i++) {
      m_x[i] = IloArray<IloNumVarArray>(m_env, m_instance->getCustomers()+2);
      for (unsigned j = 0; j < m_instance->getCustomers()+2; j++) {
         m_x[i][j] = IloNumVarArray(m_env, m_instance->getVehicles());
         for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
            m_x[i][j][k] = IloNumVar(m_env, 0, 1, ILOBOOL);
            sprintf(name, "x_%d_%d_%d", i, j, k);
            m_x[i][j][k].setName(name);
            if (i == j) {
               m_x[i][j][k].setUB(0);
            }
         }
      }
   }
   
   m_s = IloArray<IloNumVarArray>(m_env, m_instance->getCustomers()+2);
   for (unsigned i = 0; i < m_instance->getCustomers()+2; i++) {
      m_s[i] = IloNumVarArray(m_env, m_instance->getVehicles());      
      for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
         m_s[i][k] = IloNumVar(m_env, 0, IloInfinity, ILOFLOAT);
         sprintf(name, "y_%d_%d", i, k);
         m_s[i][k].setName(name);
      }
   }

   // Objective function: Min sum c_ij * x_ijk
   m_obj  = IloAdd(m_model, IloMinimize(m_env, 0, "objective"));
   for (unsigned i = 0; i < m_instance->getCustomers()+2; i++) {
      for (unsigned j = 0; j < m_instance->getCustomers()+2; j++) {
         for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
            m_obj.setLinearCoef(m_x[i][j][k], m_instance->getDistance(i, j));
         }
      }
   }
   
   // Constraints 1: sum x_ijk = 1, for all customers
   for (unsigned i = 1; i < m_instance->getCustomers()+1; i++) {
      IloExpr expr(m_env);
      for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
         for (unsigned j = 0; j < m_instance->getCustomers()+2; j++) {
            expr += m_x[i][j][k];
         }
      }
      IloConstraint constr = (expr == 1);
      sprintf(name, "C1_%d", i);
      constr.setName(name);
      m_model.add(constr);
      expr.end();
   }

   // Constraints 2: sum q_i * x_ijk <= capacity, for all vehicles
   for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
      IloExpr expr(m_env);
      for (unsigned i = 1; i < m_instance->getCustomers()+1; i++) {
         for (unsigned j = 0; j < m_instance->getCustomers()+2; j++) {
            expr += ((IloInt)m_instance->getDemand(i) * m_x[i][j][k]);
         }
      }
      IloConstraint constr = (expr <= (IloInt)m_instance->getCapacity());
      sprintf(name, "C2_%d", k);
      constr.setName(name);
      m_model.add(constr);
      expr.end();
   }

   // Constraints 3: sum x_0jk = 1, for all vehicles
   for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
      IloExpr expr(m_env);
      for (unsigned j = 1; j < m_instance->getCustomers()+2; j++) {
         expr += m_x[0][j][k];
      }
      IloConstraint constr = (expr == 1);
      sprintf(name, "C3_%d", k);
      constr.setName(name);
      m_model.add(constr);
      expr.end();
   }

   // Constraints 4: sum x_{i(n+1)k} = 1, for all vehicles 
   for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
      IloExpr expr(m_env);
      for (unsigned i = 0; i < m_instance->getCustomers()+1; i++) {
         expr += m_x[i][m_instance->getCustomers()+1][k];
      }
      IloConstraint constr = (expr == 1);
      sprintf(name, "C4_%d", k);
      constr.setName(name);
      m_model.add(constr);
      expr.end();
   }

   // Constraints 5: sum x_ihk - sum x_hjk = 0, for all customer h, vehicles
   for (unsigned h = 1; h < m_instance->getCustomers()+1; h++) {
      for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
         IloExpr expr(m_env);
         for (unsigned i = 0; i < m_instance->getCustomers()+1; i++) {
            expr += m_x[i][h][k];
         }
         for (unsigned j = 1; j < m_instance->getCustomers()+2; j++) {
            expr -= m_x[h][j][k];
         }
         IloConstraint constr = (expr == 0);
         sprintf(name, "C5_%d_%d", h, k);
         constr.setName(name);
         m_model.add(constr);
         expr.end();     
      }
   }

   // Constraints 6: s_ik + d_i + c_ij - (1 - x_ijk)*K <= s_jk
   //              = s_ik - s_jk + K * x_ijk <= K - cij
   for (unsigned i = 0; i < m_instance->getCustomers()+1; i++) {
      for (unsigned j = 1; j < m_instance->getCustomers()+2; j++) {
         for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
            if (i == j) continue;
            IloExpr expr(m_env);
            IloNum cij = m_instance->getDistance(i, j) + m_instance->getService(i);
            expr += m_s[i][k];
            expr -= m_s[j][k];
            expr += ((IloInt)K * m_x[i][j][k]);
            IloConstraint constr = (expr <= (K - cij));
            sprintf(name, "C6_%d_%d_%d", i, j, k);
            constr.setName(name);
            m_model.add(constr);
            expr.end();     
         }
      }
   }

   // Constraints 7a: s_ik >= btw_i, for all customers, vehicles
   for (unsigned i = 0; i < m_instance->getCustomers()+2; i++) {   
      for (unsigned k = 0; k < m_instance->getVehicles(); k++) {   
         IloConstraint constr = (m_s[i][k] >= (IloInt)m_instance->getBtw(i));
         sprintf(name, "C7a_%d_%d", i, k);
         constr.setName(name);
         m_model.add(constr);
      }
   }

   // Constraints 7b: s_ik <= etw_i, for all customers, vehicles
   for (unsigned i = 0; i < m_instance->getCustomers()+2; i++) {   
      for (unsigned k = 0; k < m_instance->getVehicles(); k++) {   
         IloConstraint constr = (m_s[i][k] <= (IloInt)m_instance->getEtw(i));
         sprintf(name, "C7b_%d_%d", i, k);
         constr.setName(name);
         m_model.add(constr);
      }
   }   
}

Model::~Model() {
   // empty
}

void Model::run(const unsigned timelimit, const unsigned threads, const bool verbose) {
   if (timelimit > 0) {
      m_cplex.setParam(IloCplex::TiLim, timelimit);
   }
   if (threads > 0) {
      m_cplex.setParam(IloCplex::Threads, threads);
   }
   if (!verbose) {
      m_cplex.setParam(IloCplex::MIPDisplay, 0);
      m_cplex.setParam(IloCplex::SimDisplay, 0);
   }
   
   m_cplex.exportModel("model.lp");
   
   std::chrono::system_clock::time_point start = std::chrono::system_clock::now();   
   m_cplex.solve();

   cout << "Status: " << m_cplex.getStatus() << endl;
   cout << "Obj: " << m_cplex.getObjValue() << endl;

   unsigned v = 0;
   for (unsigned j = 1; j < m_instance->getCustomers()+1; j++) {
      for (unsigned k = 0; k < m_instance->getVehicles(); k++) {
         if (m_cplex.getValue(m_x[0][j][k]) > 0.5) {
            v++;
         }
      }
   }

   cout << "Vehicles: " << v << endl;
   cout << "Time: " << (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-start).count()/1000 << endl;
}