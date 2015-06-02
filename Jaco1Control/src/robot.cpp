#include "robot.hpp"

void robot::StartAllThread()
{
	    // start the thread in the object robot_status object
		st->Start();
		// start the thread in the robot object
		this->safety_check = new boost::thread(boost::bind(&robot::Cheking,this));
}

void robot::StopAllThread()
{
	st->Stop();
	safety_check->join();
}

void robot::Cheking()
{

	while( !this->stop.load(boost::memory_order_acquire) )
	{

		// body of the method



		//---
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		{
			std::cout<< "close the checking thread"<<std::endl;
			return;
		}

	}
	std::cout<< "im out the the Cheking thread"<<std::endl;

}
