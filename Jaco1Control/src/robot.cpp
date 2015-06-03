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
	try
	{
		while( !this->stop.load(boost::memory_order_acquire) )
		{
			std::vector<std::vector<double> > cur_val;
			bool read_data = false;
			//DEBUG
			//std::cout<<"1"<<std::endl;
			//---
			for(unsigned int i = 0;i<check.checklist.size();i++)
			{
				std::vector<double> res;
				//DEBUG
				//std::cout<<"2"<<std::endl;
				 //std::cout<<check.checklist[i]<<std::endl;
				//---
				read_data = st->GetLastValue(res,check.checklist[i]);
				//DEBUG
				//std::cout<<"3"<<std::endl;
				//---
				// if the thread that publish data start to write data i will start to check them
				if(read_data)
				{
					//DEBUG
					/* std::cout<<"3.1"<<std::endl;
					 std::cout<<check.checklist[i]<<std::endl;
					 for(unsigned int ii =0;ii<res.size();ii++)
					 {
						 std::cout<<res[ii]<<" ";
					 }
						 std::cout<<std::endl;
					 //---*/
					cur_val.push_back(res);
				}
				//DEBUG
				//std::cout<<"4"<<std::endl;
				//---
			}

			if(read_data)
			{
				//DEBUG
				//std::cout<<"5"<<std::endl;
				//---
				this->check.VerifyViolation(cur_val);
			}
			// add control through interface

			//if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			//{
			//	std::cout<< "close the checking thread"<<std::endl;
			//	return;
			//}

		}
		std::cout<< "im out the the Cheking thread"<<std::endl;
	}
	catch(const std::exception &e)
	{
		std::cout<< "error in the checking thread"<<std::endl;
	}
}
