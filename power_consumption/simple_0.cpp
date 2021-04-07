/* Copyright (c) 2017-2021. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

/* This example demonstrate the actor migrations.
 *
 * The worker actor first move by itself, and then start an execution.
 * During that execution, the monitor migrates the worker, that wakes up on another host.
 * The execution was of the right amount of flops to take exactly 5 seconds on the first host
 * and 5 other seconds on the second one, so it stops after 10 seconds.
 *
 * Then another migration is done by the monitor while the worker is suspended.
 *
 * Note that worker() takes an uncommon set of parameters,
 * and that this is perfectly accepted by create().
 */

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
sg4::Host * host=sg4::this_actor::get_host();

double start=sg4::Engine::get_clock();
sg4::this_actor::sleep_for(10);
XBT_INFO( "%s ,Sleeping done for %.2f Secs. Current peak speed=%.0E; Energy dissipated=%.2f J ",sg4::this_actor::get_cname(),
		sg4::Engine::get_clock()-start,host->get_speed(), sg_host_get_consumed_energy(host));

XBT_INFO("%s, Run a computation of %.0E flops",sg4::this_actor::get_cname(), work);
start=sg4::Engine::get_clock();
  simgrid::s4u::this_actor::execute(work);

  XBT_INFO(
      "%s, Computation done (duration: %f s). Current peak speed=%.0E flop/s; Current consumption: from %.0fW to %.0fW"
      " depending on load; Energy dissipated=%.0f J",sg4::this_actor::get_cname(),
      simgrid::s4u::Engine::get_clock() - start, host->get_speed(), sg_host_get_wattmin_at(host, host->get_pstate()),
      sg_host_get_wattmax_at(host, host->get_pstate()), sg_host_get_consumed_energy(host));

}



int main(int argc, char* argv[])
{
  sg_host_energy_plugin_init();
  sg4::Engine e(&argc, argv);
  e.load_platform(argv[1]);
  std::vector<simgrid::kernel::routing::ClusterZone *> clusters = e.get_filtered_netzones<simgrid::kernel::routing::ClusterZone>();
  std::vector<simgrid::s4u::Host *> cluster_0_hosts= clusters[0]->get_all_hosts();
  std::vector<simgrid::s4u::Host *> cluster_1_hosts= clusters[1]->get_all_hosts();
  //for(auto host:cluster_0_hosts)
	// XBT_INFO("Cluster name is: %s, Host name is: %s",clusters[0]->get_cname(),host->get_cname());
  simgrid::s4u::Actor::create("actor 1", cluster_0_hosts[0], worker,1000E6);
  simgrid::s4u::Actor::create("actor 2", cluster_0_hosts[1], worker,1500E6);
  simgrid::s4u::Actor::create("actor 3", cluster_0_hosts[2], worker,2000E6);

  simgrid::s4u::Actor::create("actor 4", cluster_1_hosts[0], worker,1000E6);
  simgrid::s4u::Actor::create("actor 5", cluster_1_hosts[1], worker,2000E6);
  simgrid::s4u::Actor::create("actor 6", cluster_1_hosts[2], worker,2500E6);





 e.run();

  return 0;
}
