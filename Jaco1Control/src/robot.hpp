/*
 * robot.hpp
 *
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
			try
			{
				this->StartAllThread();
				contr->index = -1; // in this way i define the inizialization of the controller contr->index = -1
				while(!this->stop.load(boost::memory_order_acquire) )
				{
					std::vector<State> cur_val;
					bool read_data = false;
					//DEBUG
					//std::cout<<"before GetLastValue "<<std::endl;
					//std::cout<<"contr->measured_value.size() "<<contr->measured_value.size()<<std::endl;
					//----
					read_data = st->GetLastValue(cur_val,contr->measured_value);
					//DEBUG
					/*if(read_data)
					{
						std::cout<< "cur_val"<<std::endl;
						for(unsigned int ik =0;ik<cur_val[0].size();ik++)
								std::cout<<cur_val[0][ik]<<" ";
						std::cout<<std::endl;
					}*/
					//std::cout<<"read_data after GetLastValue= "<<read_data<<std::endl;
					//---

					// control block
					if(read_data)
					{
						if(contr->index == -1)
						{
							//DEBUG
							std::cout<<"before move2home"<<std::endl;
							//---
							//contr->Move2Home();
							std::vector<State> start = st->FirstRead(contr->measured_value);
							//DEBUG
							std::cout<<"after move2home"<<std::endl;
							//---
							contr->InitController(start);
							// inizialization of current value after after move2home
							cur_val = start;
							//DEBUG
							std::cout<< "starting joint position"<<std::endl;
							for(unsigned int ik =0;ik<start[0].size();ik++)
									std::cout<<start[0][ik]<<" ";
							std::cout<<std::endl;
							std::cout<< "starting cartesian position"<<std::endl;
							for(unsigned int ik =0;ik<start[1].size();ik++)
									std::cout<<start[1][ik]<<" ";
							std::cout<<std::endl;
							//---
							contr->index = 0;  //DEBUG i inibhit control  instead contr->index = 0
						}
						else if(contr->index >= 0)
						{
							//DEBUG
							//std::cout<<"im in exec controller"<<std::endl;
							//---
							bool executed_control = contr->ExecController(cur_val);
							 // using this if statement i will keep the last value when i will reach the end of this->ff vector
							 if(contr->index<(int)contr->ff[0].size()-1 && executed_control)
							 {
								 contr->index++;
								 //DEBUG
								 std::cout<<"index = "<< contr->index<<std::endl;
								 //---
							 }
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
