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
	  // ADD MUTEX
      // pure virtual function
      virtual bool GetLastValue(State& , std::string type ) = 0;
      virtual void Start() = 0;
      virtual void Stop() = 0;
      virtual void Reading() = 0;
      inline void ReadingMutex()
      {// INSERT MUTEX
    	  Reading();
    	// CLOSE MUTEX
      };
      inline bool GetLastValueMutex(State& st, std::string type)
      {// INSERT MUTEX
    	  return GetLastValue(st,type);
    	  // CLOSE MUTEX
      };
      virtual ~stats(){};
};



#endif /* STATS_HPP_ */
