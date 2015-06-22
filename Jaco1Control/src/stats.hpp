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
	  virtual std::vector<State> FirstRead() = 0;
      virtual bool GetLastValue(State& , std::string type ) = 0;
      virtual void Start() = 0;
      virtual void Stop() = 0;
      virtual void Reading() = 0;
      virtual ~stats(){};
};



#endif /* STATS_HPP_ */
