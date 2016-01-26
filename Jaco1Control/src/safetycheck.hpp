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
#include <iostream>

// to build the bounding box i have to provide the lower 3d point that is in the fourth quadrant of the xy plane of the reference system of the robot
struct box{
	std::vector<double> min_point; // position of the bottom left corner of the box in the cartesian space
	std::vector<double> max_point;  // dimension of the box in the meters
	double safe_distance; // 30 cm dalla bounding box
	box(){ safe_distance = 0.3;};
	box(std::vector<double> down_left_corner, std::vector<double> dims)
	: min_point(down_left_corner){
		max_point = down_left_corner + dims;
		safe_distance = 0.3;
	}
	bool OutBox(std::vector<double> cp){
		bool result =  box_contains_point_nd( cp.size(), &(this->min_point[0]), &(this->max_point[0]),&(cp[0]));
		return !result;
	}
	double DistBox(std::vector<double> cp){
		double res= box_point_dist_nd(cp.size(), &(this->min_point[0]), &(this->max_point[0]),&(cp[0]));
		return res;
	}
};
struct range{
	std::vector<double> min;
	std::vector<double> max;
	std::vector<double> distance_from_mid_point;
	range(){};
	range(std::vector<double> _min,std::vector<double> _max){
		min = _min;
		max = _max;

		for(unsigned int i =0;i<max.size();i++){
			distance_from_mid_point.push_back( ( ( max[i]-min[i] )/2) );
		}
	}
	std::vector<double> IsValid(State test){
		std::vector<double> result;
		// the same value range for every value
		if(min.size() == 1 && max.size() == 1){
			for(unsigned int i=0;i<test.size();i++){
			   if( test[i] > min[0] && test[i] <= max[0]){
				   result.push_back( std::min( (test[i] - min[0]),(max[0]-test[i]) ) );
			   }else{
				   result.push_back(0);
			   }
			}
		}
		//// different value ranges for every value
		else{
			for(unsigned int i=0;i<test.size();i++){
				if(test[i] > min[i] && test[i] <= max[i]){
					result.push_back( std::min( (test[i] - min[i]),(max[i]-test[i]) ) );
				}else{
					result.push_back(0);
				}
			}
		}
		return result;
	}
};

class safetycheck{
	std::vector<box> bb;
	std::map< std::string,range> rs;
	boost::mutex mutex;
	std::vector<std::string> private_checklist;
	std::vector<std::vector<double> > distances_from_violation; // measure of the distance to reach a violation condition ( [0<=] == violation  [>0] == no violation)
	std::vector<int>    score; //0 --> no violation 100 --> violation

	std::vector<double> CheckBB(State point);
	void Scoring();

public:
	bool launch_thread;
	std::vector<std::string> checklist;

	safetycheck(){
		launch_thread = false;
		// i need this to avoid problem when i execute safety check when the object is empty
		std::string empty = " ";
		for(int i=0;i<3;i++)
			checklist.push_back(empty);
		private_checklist = checklist;
	};
	safetycheck(std::vector<std::vector<double> > l_down_left_corner,std::vector<std::vector<double> > l_dims,
				std::vector<std::vector<double> > l_min,std::vector<std::vector<double> > l_max, std::vector<std::string> names){
		launch_thread = false;
		// istanziate the map with the ranges
		for(unsigned int i =0;i<names.size();i++){
			range r(l_min[i],l_max[i]);
			this->rs.insert(std::pair<std::string,range>(names[i],r));
		}
		//istanziate bounding box
		for(unsigned int i =0;i<l_down_left_corner.size();i++){
			box b(l_down_left_corner[i],l_dims[i]);
			this->bb.push_back(b);
		}
		// i save the cheklist
		checklist = names;
		// if i define a bounding box i add the bounding box in the cheklist
		if(l_down_left_corner.size()>0){
			checklist.push_back("cart_pos");
		}
		// private_checklist only for internal use;
		private_checklist = checklist;
		// initialize distances_from_violation
		for(unsigned int i=0;i<checklist.size();i++){
			std::vector<double> empty_vec;
			distances_from_violation.push_back(empty_vec);
		}
		// initialize score
		for(unsigned int i=0;i<checklist.size();i++)
		{
			this->score.push_back(0);
		}
	}
	safetycheck (const safetycheck &c){
		bb = c.bb;
		rs = c.rs;
		launch_thread = c.launch_thread;
		checklist = c.checklist;
		private_checklist = c.private_checklist;
		distances_from_violation = c.distances_from_violation;
		score = c.score;
	};
	void operator=(const safetycheck & rhs){
		bb = rhs.bb;
		rs = rhs.rs;
		launch_thread = rhs.launch_thread;
		checklist = rhs.checklist;
		private_checklist = rhs.private_checklist;
		distances_from_violation = rhs.distances_from_violation;
		score = rhs.score;
	};
	 void VerifyViolation( std::vector<State> valuelist);
     std::vector<int> GetScore();
};




#endif /* SAFETYCHECK_HPP_ */
