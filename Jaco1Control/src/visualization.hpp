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
#include <vector>
#include <plplot/plstream.h>
#include <boost/circular_buffer.hpp>
#include "common.hpp"

// data strucuture for visualization
struct data_vis
{
	int NBLOCKS;
	int NJOINTS;
	int chunk_dim;
	PLFLT x_min;
	PLFLT x_max;
	PLFLT low_thresh;
	PLFLT high_thresh;
	std::vector<PLFLT> y_min;
	std::vector<PLFLT> y_max;
	std::vector<std::string> label;
	std::vector<std::string> title;
	data_vis(){};
	data_vis(int _NBLOCKS,int _NJOINTS,int _chunk_dim,PLFLT _x_min,PLFLT _x_max,PLFLT _low_thresh,PLFLT _high_thresh,
			std::vector<PLFLT>  d_a,std::vector<PLFLT>  d_b,std::vector<std::string> s_a,std::vector<std::string> s_b)
	: y_min(d_a)
	, y_max(d_b)
	, label(s_a)
	, title(s_b)
	{
		//y_min = std::vector<PLFLT>(d_a, end(d_a)); not work
		this->NBLOCKS = _NBLOCKS;
		this->NJOINTS = _NJOINTS;
		this->chunk_dim = _chunk_dim;
		this->x_min = _x_min;
		this->x_max = _x_max;
		this->low_thresh = _low_thresh;
		this->high_thresh= _high_thresh;
	};

};

typedef std::vector<boost::circular_buffer<PLFLT> > buffer;

class visualization
{
public:
	visualization();
	visualization(int NBLOCKS,int NJOINTS,int chunk_dim,PLFLT x_min,PLFLT x_max,PLFLT low_thresh,PLFLT high_thresh,
				   std::vector<PLFLT> y_min,std::vector<PLFLT> y_max,std::vector<std::string> label,std::vector<std::string> title);
	visualization(data_vis data);

    void Plot();
    void Update(std::vector<std::vector<double>* > & lastval);

private:
    plstream *pls;
    boost::circular_buffer<PLFLT> time; // in update
    std::vector<buffer> buf; // in update
    int cur_size;            // in update;
    data_vis data;
};



#endif /* VISUALIZATION_HPP_ */
