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
#include<iostream>
namespace sg4 = simgrid::s4u;
using namespace std;
XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_actor_migration, "Messages specific for this s4u example");
std::string message;
static void sender(sg4::Mailbox *  mail_box)
{
	if(message.empty()){
		message="first message";}

	mail_box->put(new std::string(message), message.size());
	XBT_INFO("I am the Actor : %d. Data is ready in the Mailbox",sg4::this_actor::get_pid());
	message="";
	}


static void receiver(sg4::Mailbox *  mail_box)
{
	if(mail_box->empty()==true){
		//mail_box->set_receiver(simgrid::s4u::Actor::self());
		 auto *msg = static_cast<std::string*>(mail_box->get());
		 message=msg->c_str();
		 XBT_INFO("I am the Actor : %d. Data has been received: %s",sg4::this_actor::get_pid(),msg->c_str());
}}




int main(int argc, char* argv[])
{
  sg4::Engine e(&argc, argv);
  e.load_platform(argv[1]);
  sg4::Host * host_0=sg4::Host::by_name("host_0");
  sg4::Host * host_1=sg4::Host::by_name("host_1");
  sg4::Host * host_2=sg4::Host::by_name("host_2");
  sg4::Mailbox * mail_box = sg4::Mailbox::by_name("inventory_0");
 sg4::Actor::create("actor_0",host_0, sender,mail_box);
 sg4::Actor::create("actor_1", host_1, receiver,mail_box);
 sg4::Actor::create("actor_2", host_1, sender,mail_box);
 sg4::Actor::create("actor_3", host_2, receiver,mail_box);


  //sg4::Actor::create("actor_2", host_2, worker,host_0);
 e.run();

  return 0;
}
