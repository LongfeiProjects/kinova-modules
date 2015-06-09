/*
 * robot.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef ROBOT_HPP_
#define ROBOT_HPP_

#include "kinova_status.hpp"
#include "kinova_controller.hpp"
#include "safetycheck.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

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
				while(!this->stop.load(boost::memory_order_acquire) )
				{
					std::vector<State> cur_val;
					bool read_data = false;
					for(unsigned int i = 0;i<contr->measured_value.size();i++)
					{
						State res;
						read_data = st->GetLastValue(res,check.checklist[i]);
						if(read_data)
						{
							cur_val.push_back(res);
						}
					}
					// control block
					if(read_data)
					{
						if(contr->index == -1)
							contr->InitController(cur_val);
						else
							contr->ExecController(cur_val);
					}

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
					{
						std::cout<< "---------------------------------------------------------"<<std::endl;
						this->stop.store(true,boost::memory_order_release);;
						this->StopAllThread();
					}

				}
				std::cout<< "im out the main cycle"<<std::endl;
			}
			catch(const std::exception &e)
			{
				std::cout<< "something gone wrong"<<std::endl;
				this->StopAllThread();
			}
		};

		void StartAllThread();
		void StopAllThread();
		void Cheking();
};



#endif /* ROBOT_HPP_ */
