/*
 * robot.hpp
 *l
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef ROBOT_HPP_
#define ROBOT_HPP_


#include "safetycheck.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Idevice.hpp"
#include "robotmodel.hpp"

class robot
{
	public:
		boost::shared_ptr<stats>  st;
		boost::shared_ptr<controller>  contr;
		safetycheck check;
		boost::thread* safety_check;
		boost::thread* emergency_stop;

		boost::atomic<bool> stop;

		robot()
		{};

		robot(stats * _st,controller * _ct,safetycheck _check)
		: st(_st)
		,contr(_ct)
		,stop(false)
		{
			this->check = _check;
			this->safety_check = NULL;
			this->emergency_stop = NULL;
		};

		inline void Exec()
		{
		    boost::chrono::high_resolution_clock::time_point time_reference;
		    boost::chrono::milliseconds cur_time,control_time;
		    boost::chrono::high_resolution_clock::time_point begin;
			try
			{
				this->StartAllThread();
				contr->index = -1; // in this way i define the inizialization of the controller contr->index = -1
				bool read_data = false;  // debug true
				while(!this->stop.load(boost::memory_order_acquire) )
				{
					std::vector<State> cur_val;
					begin = boost::chrono::high_resolution_clock::now();
					read_data = st->GetLastValue(cur_val,contr->measured_value);
					// control block
					//debug

					//
					if(read_data) // debug
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
							// inizialization of current value after after move2home
							cur_val = start;
							//DEBUG
							/*std::cout<< "starting joint position"<<std::endl;
							for(unsigned int ik =0;ik<start[0].size();ik++)
									std::cout<<start[0][ik]<<" ";
							std::cout<<std::endl;
							std::cout<< "starting cartesian position"<<std::endl;
							for(unsigned int ik =0;ik<start[1].size();ik++)
									std::cout<<start[1][ik]<<" ";
							std::cout<<std::endl;*/
							//---
							contr->index = 0;
						}
						else if(contr->index >= 0)
						{
							bool executed_control = contr->ExecController(cur_val);
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
				this->StopAllThread();
				std::cout<< "im out the main cycle"<<std::endl;
			}
			catch(const std::exception &e)
			{
				std::cout<< "something gone wrong"<<std::endl;
				this->stop.store(true,boost::memory_order_release);
				this->StopAllThread();
				return;
			}
		};

		void StartAllThread();
		void StopAllThread();
		void Cheking();
		void EmergencyStop();
};



#endif /* ROBOT_HPP_ */
