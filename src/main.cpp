#include <iostream>
#include <cstdlib>
#include <string>
#include <dirent.h>
#include <fstream>
#include <ilcplex/ilocplex.h>
#include "Instance.hpp"
#include "Model.hpp"
#include "VNS.hpp"

using namespace std;

#include <boost/program_options.hpp>
#include <boost/format.hpp> 
namespace po = boost::program_options;
using namespace boost;

int main(int argc, char **argv){
   vector<string> iname(1);
   bool verbose = 0;
   unsigned timeLimit, threads, cplex_timelimit;

   po::options_description desc("Usage: ./cvrptw instance_file [Options]\nOptions");
   desc.add_options()
      ("help", "show help")
      ("exact,e", "solve with cplex")
      ("instance,i", po::value<vector<string>>(&iname), "instance file")
      ("timelimit,t", po::value<unsigned>(&timeLimit), "time limit in seconds")
      ("threads", po::value<unsigned>(&threads)->default_value(0), "number of threads to use")
      ("cplex_timelimit", po::value<unsigned>(&cplex_timelimit)->default_value(0), "cplex time limit in seconds")
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

	// Initialize seed for the random number generator
	std::srand ( unsigned ( std::time(0) ) );

	// Iterate over the directory of Cordeau's instances to solve each of them using VNS
	struct dirent *pDirent; DIR *instancesDir;
	instancesDir = opendir ("../../instances/cordeau");
	// Write the results to a .dat file
	ofstream results ("../../results/VNS/cordeau.dat");
	results << "filename & customers & vehicles & opt_vehicles & opt_dist & tonn_vehicles & tonn_dist & dev_tonn_vehicles & dev_tonn_dist & tonn_time & VNS_vehicles & VNS_dist & dev_VNS_vehicles & dev_VNS_dist & VNS_time" << endl;
	int i = 0;
	while ((pDirent = readdir(instancesDir)) != NULL) 
	{
		string filename = pDirent->d_name;
		if( filename.find(".") == string::npos )
		{
			// Search the directory of optimum solutions for a file named 'filename'.res
			// Write that solution's vehicle count and total dist in variables vopt and dopt
			DIR *optimaDir = opendir("../../optima/cordeau");
			int vopt = 0;
			double dopt;
			while((pDirent = readdir(optimaDir)) != NULL)
			{
				string filename2 = pDirent->d_name;
				if( filename2.compare(0,filename2.length()-4,filename) == 0 )
				{
					ifstream optimum ((boost::format("../../optima/cordeau/%s") % filename2).str());
					optimum >> dopt;

					string line;
					vopt = 0;
					while(getline(optimum,line))
					{
						if(line.length() > 1)
						{
							vopt++;
						}
					}

					break;
				}
			}

			string path = (boost::format("../../instances/cordeau/%s") % filename).str(); //"../../instances/cordeau/";

			Instance* inst = new Instance();
			inst->loadFromFile(path);

			results << filename << " " << inst->getCustomers() << " " << inst->getVehicles() << " " << vopt << " " << dopt << " ";

			VNS vns(inst);
			vns.run(verbose, vopt, dopt, results);

			delete inst;

			i++; 
		}
	}
	closedir (instancesDir);
	results.close();
   
   /*
   // cout << iname[0] << endl;
   Instance* inst =  new Instance();
   inst->loadFromFile(iname[0]);
   // cout << "Instance loaded!" << endl;
   // inst->print();

   if (vm.count("exact")) {
      IloEnv env;
      Model m(env, inst);
      m.run(cplex_timelimit, threads, verbose);
      env.end();
   }
   else {
      VNS vns(inst);
      vns.run(verbose);
   }

   delete inst;
   */
   return 0;
}
