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
			this->StartAllThread();
			while(!this->stop.load(boost::memory_order_acquire) )
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
				{
					std::cout<< "---------------------------------------------------------"<<std::endl;
					this->stop.load(boost::memory_order_acquire);
					this->StopAllThread();
				}

			}
			std::cout<< "im out the main cycle"<<std::endl;
		};

		void StartAllThread();
		void StopAllThread();
		void Cheking();
};



#endif /* ROBOT_HPP_ */
