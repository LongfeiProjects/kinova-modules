/*
 * visualization.hpp
 *
 *  Created on: May 27, 2015
 *      Author: vale
 */

#ifndef VISUALIZATION_HPP_
#define VISUALIZATION_HPP_

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cmath>
#include <plplot/plstream.h>
#include <boost/circular_buffer.hpp>



typedef std::vector<boost::circular_buffer<PLFLT> > buffer;

class visualization
{
public:
	visualization();
	visualization(int NJOINTS,int chunk_dim,PLFLT x_min,PLFLT x_max,PLFLT low_thresh,PLFLT high_thresh,std::vector<PLFLT> y_min, std::vector<PLFLT>y_max,
					std::vector<std::string> label,std::vector<std::string> title);
    void Plot(int blocks);
    void Update(std::vector<std::vector<double>* > lastval);

private:
    plstream *pls;
    boost::circular_buffer<PLFLT> time; // in update
    std::vector<buffer> buf; // in update
    PLFLT x_min,x_max;       // in update
    int cur_size;            // in update;

    int chunk_dim;
    int NJOINTS;
    PLFLT low_thresh, high_thresh;
    std::vector<PLFLT> y_min,y_max;
    std::vector<std::string> label,title;
};


#endif /* VISUALIZATION_HPP_ */
