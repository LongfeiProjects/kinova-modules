/*
 * controller.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */


#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#include "common.hpp"
#include "model.hpp"
class controller
{
	private:

	public:
		double P;
		double I;
		double D;
		double time_interval; // controller frequency
		int index;  // current value
		std::vector<State> last_current_values; // variable to storage values for control purpose;
		std::vector<std::string> measured_value; // vector of string that describe the value that want to measure and in which order we want them
		model * bot;

    	virtual int Move2Home() = 0;
    	virtual bool InitController(std::vector<State> initial_state) = 0;
		virtual bool ExecController(std::vector<State> current_state) = 0;
		virtual void SendSingleCommand(State cmd) = 0;
		virtual State PID(std::vector<State> ff,std::vector<State> current_state) = 0;
		virtual ~controller(){};

		inline int ReadFile(std::string namefile,std::vector< State > & value)
		{
			std::ifstream infile;
			int Njoints;

			try
			{
			  infile.open(namefile.c_str(),std::ifstream::in);

			  std::string line;
			  // in the first line is defined the number of joints of the robot
			  std::getline(infile, line);
			  std::stringstream ss1(line);
			  if ( !(ss1 >> Njoints) )
			  {
				  std::cout<<"problem reading number of joint"<< std::endl;
			  }

			  while (std::getline(infile, line))
			  {
					std::stringstream ss(line);
					State app(Njoints);
					int index = 0;
					while( !ss.eof() )
					{
						double ff;
						if ( ss >> ff)
						{
						   app[index] = ff;
						   index++;
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

			return Njoints;
		}

};


#endif /* CONTROLLER_HPP_ */
