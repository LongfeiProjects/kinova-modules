/*
 * safetycheck.hpp
 *
 *  Created on: Jun 1, 2015
 *      Author: vale
 */

#ifndef SAFETYCHECK_HPP_
#define SAFETYCHECK_HPP_

#include "geometry.hpp"
#include "common.hpp"
#include <map>

struct box
{
	std::vector<double> min_point; // position of the bottom left corner of the box in the cartesian space
	std::vector<double> max_point;  // dimension of the box in the meters
	box(){};
	box(std::vector<double> down_left_corner, std::vector<double> dims)
	: min_point(down_left_corner)
	{
		max_point = down_left_corner + dims;
	}

	bool OutBox(std::vector<double> cp)
	{
		bool result =  box_contains_point_nd( cp.size(), &(this->min_point[0]), &(this->max_point[0]),&(cp[0]));
		return !result;
	}

};


struct range
{
	std::vector<double> min;
	std::vector<double> max;
	range(){};
	range(std::vector<double> _min,std::vector<double> _max)
	{
		min = _min;
		max = _max;
	}

	bool IsValid(std::vector<double> test)
	{
		if(min.size() == 1 && max.size() == 1)
		{
			for(unsigned int i=0;i<test.size();i++)
			{
				bool valid = test[i] > min[0] && test[i] <= max[0];
				if(!valid)
					return false;
			}
		}
		else
		{
			for(unsigned int i=0;i<test.size();i++)
			{
				bool valid = test[i] > min[i] && test[i] <= max[i];
				if(!valid)
					return false;
			}
		}
		return true;
	}
};

class safetycheck
{

	std::vector<box> bb;
	std::map< std::string,range> rs;

public:
	std::vector<std::string> checklist;

	safetycheck(){};
	safetycheck(std::vector<std::vector<double> > l_down_left_corner,std::vector<std::vector<double> > l_dims,
				std::vector<std::vector<double> > l_min,std::vector<std::vector<double> > l_max, std::vector<std::string> names)
	{

		// istanziate the map
		for(unsigned int i =0;i<names.size();i++)
		{
			range r(l_min[i],l_max[i]);
			this->rs.insert(std::pair<std::string,range>(names[i],r));
		}
		//istanziate bounding box
		for(unsigned int i =0;i<l_down_left_corner.size();i++)
		{
			box b(l_down_left_corner[i],l_dims[i]);
			this->bb.push_back(b);
		}

		// i save the cheklist
		checklist = names;
		// if i define a bounding box i add the bounding box in the cheklist
		if(l_down_left_corner.size()>0)
		{
			checklist.push_back("f_tau");
		}

	}
	 bool VerifyViolation( std::vector< std::vector<double > > valuelist);


};




#endif /* SAFETYCHECK_HPP_ */
