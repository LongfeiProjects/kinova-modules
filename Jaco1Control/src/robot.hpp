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
		};

		inline void Exec()
		{
			try
			{
				this->StartAllThread();
				contr->index = -1; // in this way i define the inizialization of the controller
				while(!this->stop.load(boost::memory_order_acquire) )
				{
					std::vector<State> cur_val;
					bool read_data = false;
					for(unsigned int i = 0;i<contr->measured_value.size();i++)
					{
						State res;
						read_data = st->GetLastValue(res,contr->measured_value[i]);
						//DEBUG
						//std::cout<<"read_data= "<<read_data<<std::endl;
						//---
						if(read_data)
						{
							cur_val.push_back(res);
						}
					}
					// control block
					if(read_data)
					{
						if(contr->index == -1)
						{
							//DEBUG
							std::cout<<"before move2home"<<std::endl;
							//---
							contr->Move2Home();
							std::vector<State> start = st->FirstRead();
							//DEBUG
							std::cout<<"after move2home"<<std::endl;
							//---
							contr->InitController(start);
							contr->index = 0;
						}
						else
						{
							//DEBUG
							//std::cout<<"im in exec controller"<<std::endl;
							//---
							contr->ExecController(cur_val);
							 // using this if statement i will keep the last value when i will reach the end of this->ff vector
							 if(contr->index<(int)contr->ff.size()-1)
								 contr->index++;
							 //DEBUG
							 std::cout<<"index = "<< contr->index<<std::endl;
							 //---
						}
					}

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
					{
						std::cout<< "---------------------------------------------------------"<<std::endl;
						this->stop.store(true,boost::memory_order_release);
						this->StopAllThread();
					}

				}
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
};



#endif /* ROBOT_HPP_ */
