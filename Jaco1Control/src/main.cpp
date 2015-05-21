
#include "kinova.hpp"



int main()
{
	bool stop=false;
	std::string namefile = "ff.txt";
	bool limitation = 0;
	kinenv::kinova mybot;

    //mybot.Move2Home();
	//mybot.FeedForward(namefile,limitation);

	//DEBUG
	//std::cout<<"before exec"<<std::endl;
	//----

	//mybot.Exec();

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
		    mybot.stats.CloseThread();
		}

	}


	std::cout<< "im out the main cycle"<<std::endl;


	return 1;


}
