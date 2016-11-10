#include <iostream>
#include <cstdlib>
#include <string>
#include <ilcplex/ilocplex.h>
#include "Instance.hpp"
#include "Model.hpp"

using namespace std;

#include <boost/program_options.hpp>
namespace po = boost::program_options;
using namespace boost;

int main(int argc, char **argv){
   vector<string> iname(1);
   bool verbose = 0;
   unsigned timeLimit, threads, cplex_timelimit;

   po::options_description desc("Usage: ./cvrptw instance_file [Options]\nOptions");
   desc.add_options()
      ("help", "show help")
      ("instance,i", po::value<vector<string>>(&iname), "instance file")
      ("timelimit,t", po::value<unsigned>(&timeLimit), "time limit in seconds")
      ("threads", po::value<unsigned>(&threads)->default_value(4), "number of threads to use")
      ("cplex_timelimit", po::value<unsigned>(&cplex_timelimit)->default_value(10), "cplex time limit in seconds")
      ("verbose,v", "verbose output")
      ;

   po::positional_options_description pod;
   pod.add("instance", 1);

   po::variables_map vm;
   po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
   po::notify(vm);

   if (vm.count("help") || !vm.count("instance")) {
      cout << desc << "\n";
      return 1;
   }

   if (vm.count("verbose")) {
      verbose = 1;
   }


   // cout << iname[0] << endl;
   Instance* inst =  new Instance();
   inst->loadFromFile(iname[0]);
   // cout << "Instance loaded!" << endl;
   // inst->print();

   IloEnv env;
   Model m(env, inst);


   delete inst;
   return 0;
}
