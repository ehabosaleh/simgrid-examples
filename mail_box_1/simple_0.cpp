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
	XBT_INFO("I am the Actor : %d. Data is ready in the Mailbox ",sg4::this_actor::get_pid());
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
  sg4::Host * host_0=sg4::Host::by_name("host_c_0_1");
  sg4::Host * host_1=sg4::Host::by_name("host_c_2_2");
  sg4::Host * host_2=sg4::Host::by_name("host_c_3_50");
  sg4::Mailbox * mail_box = sg4::Mailbox::by_name("inventory_0");
 sg4::Actor::create("actor_0",host_0, sender,mail_box);
 sg4::Actor::create("actor_1", host_1, receiver,mail_box);
 sg4::Actor::create("actor_2", host_1, sender,mail_box);
 sg4::Actor::create("actor_3", host_2, receiver,mail_box);


  //sg4::Actor::create("actor_2", host_2, worker,host_0);
 e.run();

  return 0;
}
