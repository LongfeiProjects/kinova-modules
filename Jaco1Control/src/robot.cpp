#include "robot.hpp"

void robot::StartAllThread()
{
	    // start the thread in the object robot_status object
		st->Start();
		// start the thread in the robot object
		if(check.launch_tread)
			this->safety_check = new boost::thread(boost::bind(&robot::Cheking,this));
}

void robot::StopAllThread()
{
	st->Stop();
	if(check.launch_tread)
		safety_check->join();
}

void robot::Cheking()
{
	try
	{
		while( !this->stop.load(boost::memory_order_acquire) )
		{
			std::vector<State> cur_val;
			bool read_data = false;

			read_data = st->GetLastValue(cur_val,check.checklist);
			// if the thread that publish data start to write data i will start to check them

			if(read_data)
			{
				bool check=false;
				check = this->check.VerifyViolation(cur_val);
				// add control to stop robot if i violate something
				if(check)
				{
					this->stop.store(true,boost::memory_order_release);
					this->StopAllThread();
				}

			}
			// add control through interface
			/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				std::cout<< "close the checking thread"<<std::endl;
				return;
			}*/

		}
		std::cout<< "im out the the Cheking thread"<<std::endl;
	}
	catch(const std::exception &e)
	{
		std::cout<< "error in the checking thread"<<std::endl;
	}
}
