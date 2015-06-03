/*
 * stats.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef STATS_HPP_
#define STATS_HPP_

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


class stats
{
   public:
      // pure virtual function
      virtual bool GetLastValue(std::vector<double>& , std::string type ) = 0;
      virtual void Start() = 0;
      virtual void Stop() = 0;
      virtual ~stats(){};
};



#endif /* STATS_HPP_ */
