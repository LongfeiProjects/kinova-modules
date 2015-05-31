/*
 * controller.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */


#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>

class controller
{

public:
	virtual bool ExecController() = 0;
	virtual ~controller(){};

	inline int ReadFile(std::string namefile,std::vector< std::vector<double> > & value)
	{
		std::ifstream infile;
		int traj;

		try
	    {
		  infile.open(namefile.c_str(),std::ifstream::in);

		  std::string line;

		  // in the first line is defined the kind of trajectory
		  std::getline(infile, line);
		  std::stringstream ss1(line);
		  if ( !(ss1 >> traj) )
		  {
		     std::cout<<"problem reading the kind of control"<< std::endl;
		  }

		  while (std::getline(infile, line))
		  {
				std::stringstream ss(line);
				std::vector<double> app;
				while( !ss.eof() )
				{
					double ff;
					if ( ss >> ff)
					{
					   app.push_back(ff);
					}

				}
				if(!app.empty())
				{
					value.push_back(app);
				}


		  }
		  infile.close();
	    }
	    catch (std::ifstream::failure e)
	    {
		  std::cerr << "Exception opening/reading/closing file\n";
	    }

	    return traj;
	}


};


#endif /* CONTROLLER_HPP_ */
