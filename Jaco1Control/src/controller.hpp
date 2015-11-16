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
		std::vector<std::vector<State> > ff;
		std::vector<State> desired_values; // variable to storage desired value plus other things;
		std::vector<std::string> measured_value; // vector of string that describe the value that want to measure and in which order we want them
		model * bot;
		/*the empty constructor inhibit the use of the constructor that never initialize in
		  this way because of this loc measured_value.size() for(unsigned int i = 0;i<contr->measured_value.size();i++)
		  contr->measured_value.size() is empty */
    	virtual int Move2Home() = 0;
    	virtual bool InitController(std::vector<State> initial_state) = 0;
		virtual bool ExecController(std::vector<State> current_state,int type) = 0;
		// if a set int type = -1 it means that I'm going to use the control type specified inside the controller
		virtual void SendSingleCommand(State cmd, int type) = 0;
		virtual ~controller(){};
		void  InitCartesianKinematicController(std::vector<State> initial_state);
		State CartesianKinematicController(std::vector<State> current_state);
		inline int ReadFile(std::string namefile,std::vector< State > & value)
		{
			std::ifstream infile;
			int task_space_dim;
			try
			{
			  infile.open(namefile.c_str(),std::ifstream::in);
			  std::string line;
			  // in the first line is defined the taskspacedimension
			  std::getline(infile, line);
			  std::stringstream ss1(line);
			  if ( !(ss1 >> task_space_dim) )
			  {
				  std::cout<<"problem reading number of joint"<< std::endl;
			  }
			  while (std::getline(infile, line))
			  {
					std::stringstream ss(line);
					State app(task_space_dim);
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
			return task_space_dim;
		}
};

#endif /* CONTROLLER_HPP_ */
