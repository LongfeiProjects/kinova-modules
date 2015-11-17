#include "robot.hpp"

void robot::ReadCurrentState(std::vector<State>& stat, std::vector<std::string> & type )
{
	this->st->GetLastValue(stat,type);
}

void robot::StartLog(std::vector<std::string>  & type)
{
  this->st->StartSaving(type);
}
std::vector<Log> robot::StopLog(std::vector<std::string>  & type)
{
	std::vector<Log> result = this->st->StopSaving(type);
	return result;
}


void robot::ExecuteTrajectory()
{
	boost::chrono::high_resolution_clock::time_point time_reference;
	boost::chrono::milliseconds cur_time,control_time;
	boost::chrono::high_resolution_clock::time_point begin;
	// i have to grant access to the execution of the task
	this->stop.store(false,boost::memory_order_release);
	try
	{
		contr->index = -1; // in this way i define the initialization of the controller contr->index = -1
		bool read_data = false;  // debug true
		while( !this->stop.load(boost::memory_order_acquire) )
		{
			std::vector<State> cur_val;
			begin = boost::chrono::high_resolution_clock::now();
			read_data = st->GetLastValue(cur_val,contr->measured_value);
			// control block
			if(read_data)
			{
				if(contr->index == -1)
				{
					time_reference = boost::chrono::high_resolution_clock::now();
					//DEBUG
					std::cout<<"before move2home"<<std::endl;
					//---
					//contr->Move2Home();
					std::vector<State> start;
					start = st->FirstRead(contr->measured_value);
					//DEBUG
					std::cout<<"after move2home"<<std::endl;
					//---
					contr->InitController(start);
					// Initialization of current value after after move2home
					cur_val = start;
					contr->index = 0;
				}
				else if(contr->index >= 0)
				{
					contr->ExecController(cur_val,-1);
					control_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin);
					std::cout << "time spent in main cycle: " << control_time.count()  << " ms\n";
					int test_time = boost::chrono::round<boost::chrono::milliseconds>(control_time).count();
					if(test_time < 10)
					{
						usleep(1000*((3)));
					}
					cur_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - time_reference);
					contr->index = boost::chrono::round<boost::chrono::milliseconds>(cur_time).count();
				}
			}
		}
		std::cout<< "im out the main cycle"<<std::endl;
	}
	catch(const std::exception &e)
	{
		std::cout<< "something gone wrong"<<std::endl;
		return;
	}
};

void robot::SendCommand(State cmd,int type)
{
	this->contr->SendSingleCommand(cmd,type);
}

void robot::MoveHome()
{
	this->contr->Move2Home();
}

void robot::StartAllThread()
{
	// start the thread in the object robot_status object
	st->Start();
	this->emergency_stop = new boost::thread(boost::bind(&robot::EmergencyStop,this));
	// start the thread in the robot object
	if(check.launch_tread)
		this->safety_check = new boost::thread(boost::bind(&robot::Cheking,this));
}

void robot::StopAllThread()
{
	st->Stop();
	this->stop_auxiliary_thread.store(true,boost::memory_order_release);
	emergency_stop->join();
	if(check.launch_tread)
		safety_check->join();
}

void robot::Cheking()
{
	try
	{
		while( !this->stop_auxiliary_thread.load(boost::memory_order_acquire) )
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
					//this->StopAllThread();
				}

			}
			usleep(1000*((3)));
		}
		std::cout<< "im out the the Cheking thread"<<std::endl;
	}
	catch(const std::exception &e)
	{
		std::cout<< "error in the checking thread"<<std::endl;
	}
}

void robot::EmergencyStop()
{
	while( !this->stop_auxiliary_thread.load(boost::memory_order_acquire) )
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			std::cout<< "---------------------------------------------------------"<<std::endl;
			this->st->ClearCommands();
			this->stop.store(true,boost::memory_order_release);
		}
	}
}
