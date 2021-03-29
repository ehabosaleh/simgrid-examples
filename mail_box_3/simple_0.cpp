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
#include<iostream>
#include<stdio.h>
namespace sg4 = simgrid::s4u;
using namespace std;
XBT_LOG_NEW_DEFAULT_CATEGORY(ClusterZone, "Message broadcasting");
std::string message;
static void sender(sg4::Mailbox *  mail_box)
{
	if(message.empty()){
		message="*PASS THIS MESSAGE*";}

	mail_box->put(new std::string(message), message.size());
	XBT_INFO("I am the Actor : %s. Data is ready in the Mailbox: %s ",sg4::this_actor::get_cname(),mail_box->get_cname());
	message="";
	}


static void receiver(sg4::Mailbox *  mail_box)
{
	if(mail_box->empty()==true){
		//mail_box->set_receiver(simgrid::s4u::Actor::self());
		 auto *msg = static_cast<std::string*>(mail_box->get());
		 message=msg->c_str();
		 XBT_INFO("I am the Actor : %s. Data has been received: %s\n",sg4::this_actor::get_cname(),msg->c_str());
}}




int main(int argc, char* argv[])
{
  sg4::Engine e(&argc, argv);
  e.load_platform(argv[1]);
  std::vector<simgrid::kernel::routing::ClusterZone *> clusters = e.get_filtered_netzones<simgrid::kernel::routing::ClusterZone>();
  std::vector<simgrid::s4u::Host *> cluster_0_hosts= clusters[0]->get_all_hosts();
  std::vector<simgrid::s4u::Host *> cluster_1_hosts= clusters[1]->get_all_hosts();
  std::vector<simgrid::s4u::Host *> cluster_2_hosts= clusters[2]->get_all_hosts();
  std::vector<simgrid::s4u::Host *> cluster_3_hosts= clusters[3]->get_all_hosts();
//  for(auto host:cluster_0_hosts)

	//  XBT_INFO("Cluster name is: %s, Host name is: %s",clusters[0]->get_cname(),host->get_cname());

  sg4::Mailbox * mail_box = sg4::Mailbox::by_name("inventory_0");


  for(int i=0;i<cluster_0_hosts.size()-1;i++){
	  sg4::Actor::create(cluster_0_hosts[i]->get_cname(),cluster_0_hosts[i],sender,mail_box);
	  sg4::Actor::create(cluster_0_hosts[i+1]->get_cname(),cluster_0_hosts[i+1],receiver,mail_box);
	 // cout<<i<<endl;
  }
  sg4::Actor::create(cluster_0_hosts[cluster_0_hosts.size()-1]->get_cname(),cluster_0_hosts[cluster_0_hosts.size()-1],sender,mail_box);
  sg4::Actor::create(cluster_1_hosts[0]->get_cname(),cluster_1_hosts[0],receiver,mail_box);
  for(int i=0;i<cluster_1_hosts.size()-1;i++){
	  sg4::Actor::create(cluster_1_hosts[i]->get_cname(),cluster_1_hosts[i],sender,mail_box);
	  sg4::Actor::create(cluster_1_hosts[i+1]->get_cname(),cluster_1_hosts[i+1],receiver,mail_box);

  }
  sg4::Actor::create(cluster_1_hosts[cluster_1_hosts.size()-1]->get_cname(),cluster_1_hosts[cluster_1_hosts.size()-1],sender,mail_box);
  sg4::Actor::create(cluster_2_hosts[0]->get_cname(),cluster_2_hosts[0],receiver,mail_box);
  for(int i=0;i<cluster_2_hosts.size()-1;i++){
	  sg4::Actor::create(cluster_2_hosts[i]->get_cname(),cluster_2_hosts[i],sender,mail_box);
	  sg4::Actor::create(cluster_2_hosts[i+1]->get_cname(),cluster_2_hosts[i+1],receiver,mail_box);

  }

  sg4::Actor::create(cluster_2_hosts[cluster_2_hosts.size()-1]->get_cname(),cluster_2_hosts[cluster_2_hosts.size()-1],sender,mail_box);
  sg4::Actor::create(cluster_3_hosts[0]->get_cname(),cluster_3_hosts[0],receiver,mail_box);
  for(int i=0;i<cluster_3_hosts.size()-1;i++){
	  sg4::Actor::create(cluster_3_hosts[i]->get_cname(),cluster_3_hosts[i],sender,mail_box);
	  sg4::Actor::create(cluster_3_hosts[i+1]->get_cname(),cluster_3_hosts[i+1],receiver,mail_box);

  }
 e.run();

  return 0;
}
