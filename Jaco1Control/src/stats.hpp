/*
 * stats.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef STATS_HPP_
#define STATS_HPP_

#include "common.hpp"
#include "model.hpp"

class stats
{
   public:
	  model* bot;
      // pure virtual function
	  virtual std::vector<State> FirstRead(std::vector<std::string> ) = 0;
      virtual bool GetLastValue(std::vector<State>& , std::vector<std::string> & type ) = 0;
      virtual void Start() = 0;
      virtual void Stop() = 0;
      virtual void Reading() = 0;
      virtual void StartSaving(std::vector<std::string>  & type) = 0;
      virtual std::vector<Log> StopSaving(std::vector<std::string>  & type) = 0;
      virtual ~stats(){};
};



#endif /* STATS_HPP_ */
