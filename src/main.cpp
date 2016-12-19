#include <iostream>
#include <cstdlib>
#include <string>
#include <dirent.h>
#include <fstream>
#include <ilcplex/ilocplex.h>
#include "Instance.hpp"
#include "Model.hpp"
#include "ILS.hpp"

using namespace std;

#include <boost/program_options.hpp>
#include <boost/format.hpp> 
namespace po = boost::program_options;
using namespace boost;

int main(int argc, char **argv){
   vector<string> iname(1);
   bool verbose = 0;
   unsigned timeLimit, threads, cplex_timelimit, ils_stagn, vns_stagn;
   double pf;

   po::options_description desc("Usage: ./cvrptw instance_file [Options]\nOptions");
   desc.add_options()
      ("help", "show help")
      ("exact,e", "solve with cplex")
      ("instance,i", po::value<vector<string>>(&iname), "instance file")
      ("timelimit,t", po::value<unsigned>(&timeLimit)->default_value(600), "time limit in seconds")
      ("threads", po::value<unsigned>(&threads)->default_value(0), "number of threads to use")
      ("cplex_timelimit", po::value<unsigned>(&cplex_timelimit)->default_value(0), "cplex time limit in seconds")
      ("verbose,v", "verbose output")
      ("ils_stagn", po::value<unsigned>(&ils_stagn)->default_value(200), "ILS stopping criterion (consecutive iterations without improvement)")
      ("ils_pf", po::value<double>(&pf)->default_value(0.1), "ILS perturbation factor [0,1]")
      ("vns_stagn", po::value<unsigned>(&vns_stagn)->default_value(50), "VNS stopping criterion (consecutive iterations without improvement)")
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

   if (pf < 0 || pf > 1.0) {
      pf = 0.1;
   }
   
	// Initialize seed for the random number generator
	std::srand ( unsigned ( std::time(0) ) );
   
   Instance* inst =  new Instance();
   inst->loadFromFile(iname[0]);

   if (vm.count("exact")) {
      IloEnv env;
      Model m(env, inst);
      m.run(cplex_timelimit, threads, verbose);
      env.end();
   }
   else {
      ILS ils(inst);
      ils.run(verbose, timeLimit, pf, ils_stagn, vns_stagn);
   }

   delete inst;
   
   return 0;
}
