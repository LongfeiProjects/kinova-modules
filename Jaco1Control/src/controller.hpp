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
// TO DO add a function to convert control_action from a general scheme of definition to the specific value for the current driver
class controller
{

	public:
		double P;
		double I;
		double D;
		double time_interval; // controller frequency
		int index;  // current value
        Option opt; // structure that is defined to control the behaviour of each controller (for now only the kinematic controller)
        std::vector<int> time_map;    // this vector represent the mapping between the timestamp and a sampling time of 1 ms
        std::vector<std::vector<State> > ff; // vector that contains the value of thr desired trajectory to reproduce
		std::vector<State> desired_values; // variable to storage desired value plus other things;
        //std::vector<std::string> measured_value; // vector of string that describe the value that want to measure and in which order we want them
        std::string timestamp_file;
        std::vector<std::string> ff_files;
        model * bot;
		/*the empty constructor inhibit the use of the constructor that never initialize in
		  this way because of this loc measured_value.size() for(unsigned int i = 0;i<contr->measured_value.size();i++)
		  contr->measured_value.size() is empty */
    	virtual int Move2Home() = 0;
        // init controller HAS TO BE CALLED INSIDE THE CONSTRUCTOR of each instance of the virtual controller class
        virtual bool InitController();
        virtual bool ExecController(std::vector<State> current_state,int type);
		// if a set int type = -1 it means that I'm going to use the control type specified inside the controller
		virtual void SendSingleCommand(State cmd, int type) = 0;
        virtual void SetNewFF(std::vector< std::vector<State> > new_ff){}
        virtual ~controller(){}
        // implemented methods
        void  InitCartesianKinematicController(std::vector<State> initial_state){}
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


        void ComputeTimeMap(std::vector<State> timestamps ){
            int ind = 0;
            int diff;int diff_prev;
            this->time_map.clear();
            for(int i=0; i< timestamps.back()[0];i++){
                diff = timestamps[ind][0]-i;
                if( diff == 0 || ind == 0)
                {
                    this->time_map.push_back(ind);
                    ind++;
                }else
                {
                     diff_prev = abs(timestamps[ind-1][0]-i);
                    if(diff_prev<= diff)
                        this->time_map.push_back(ind-1);
                    else
                        this->time_map.push_back(ind);
                }

            }
        }
};

#endif /* CONTROLLER_HPP_ */
