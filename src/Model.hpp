#pragma once

#include <vector>
#include <random>
#include <chrono>
#include <ilcplex/ilocplex.h>
#include "Instance.hpp"
// #include "Solution.hpp"


class Model {
public:
   Model(IloEnv &env, Instance* inst);
   ~Model();

   void run(const unsigned timelimit, const unsigned threads, const bool verbose);

   typedef IloArray<IloNumVarArray> NumVarMatrix;
   typedef IloArray<NumVarMatrix> NumVar3Matrix;
   
private:
   Instance* m_instance;

   // CPLEX
   IloEnv m_env;
   IloModel m_model;
   IloCplex m_cplex;
   IloObjective m_obj;

   // Variables
   NumVar3Matrix m_x;
   NumVarMatrix m_s;
 };
