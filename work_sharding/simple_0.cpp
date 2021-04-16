
#include <simgrid/s4u.hpp>
#include "simgrid/kernel/routing/ClusterZone.hpp"
#include "simgrid/kernel/routing/DragonflyZone.hpp"
#include "simgrid/plugins/energy.h"
#include<iostream>
#include<stdio.h>
namespace sg4 = simgrid::s4u;
using namespace std;
XBT_LOG_NEW_DEFAULT_CATEGORY(ClusterZone, "Power Consumption");
static void worker(double work){
double start=sg4::Engine::get_clock();
sg4::this_actor::execute(work);
XBT_INFO("Work is hone in the actor : %s;\n Duration: %f Seconds",sg4::this_actor::get_cname(),(sg4::Engine::get_clock()-start));
}



int main(int argc, char* argv[])
{
  sg_host_energy_plugin_init();
  sg4::Engine e(&argc, argv);
  e.load_platform(argv[1]);
  std::vector<simgrid::kernel::routing::ClusterZone *> clusters = e.get_filtered_netzones<simgrid::kernel::routing::ClusterZone>();
  int clusters_num=clusters.size();
  int hosts_num=0;
  for(int i=0; i<clusters_num;i++)
	  for(auto host:clusters[i]->get_all_hosts()){
		  host->turn_off();
		  hosts_num+=1;
		  //XBT_INFO("Cluster name is: %s, Host name is: %s",clusters[i]->get_cname(),host->get_cname());
		  }
   XBT_INFO("Clusters number is %d, Hosts number is %d:",clusters_num,hosts_num);
   double work_to_do=1000E12;

   //simgrid::s4u::Actor::create("actor 1", simgrid::s4u::Host::by_name("root"), worker,work_to_do);
   XBT_INFO("After propagate the work among all hosts:\n");
   XBT_INFO("%f",work_to_do/hosts_num);
   double shard_per_host=work_to_do/hosts_num;
   std::string actor_name="";
   for(int i=0; i<clusters_num;i++)
	  for(auto host:clusters[i]->get_all_hosts()){
		  actor_name="Actor " + host->get_name();
		  host->turn_on();
		  simgrid::s4u::Actor::create(actor_name, host, worker,shard_per_host);}

 e.run();

  return 0;
}
