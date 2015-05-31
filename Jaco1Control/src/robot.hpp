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
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class robot
{
	public:
		boost::shared_ptr<stats>  st;
		boost::shared_ptr<controller>  contr;

		robot()
		{};

		robot(stats * _st,controller * _ct)
		: st(_st)
		,contr(_ct)
		{};

		inline void Exec()
		{
			bool stop = false;
			st->Start();
			while(!stop)
			{
				//usleep(SLEEP_INTERVAL);
				//DEBUG
				//std::cout<< "im in the main cycle"<<std::endl;
				//---
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
				{
					std::cout<< "---------------------------------------------------------"<<std::endl;
					stop = true;
					st->Stop();
				}

			}
			std::cout<< "im out the main cycle"<<std::endl;
		};

};



#endif /* ROBOT_HPP_ */
