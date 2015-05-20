
#include "kinova.hpp"



int main()
{
	bool stop=false;
	std::string namefile = "ff.txt";
	bool limitation = 0;
	kinenv::kinova mybot = kinenv::kinova();

    //mybot.Move2Home();
	//mybot.FeedForward(namefile,limitation);

	//DEBUG
	std::cout<<"before exec"<<std::endl;
	//----

	//mybot.Exec();

	while(!stop)
	{
		usleep(SLEEP_INTERVAL);
		std::cout<< "sono dentro il ciclo nel main"<<std::endl;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			std::cout<< "---------------------------------------------------------"<<std::endl;
		    stop = true;
		    mybot.stats.StopRun();
		}

	}


	std::cout<< "sono fuori da ciclo nel main"<<std::endl;
	mybot.~kinova();


	return 1;


}
