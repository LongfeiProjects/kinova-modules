#include "safetycheck.hpp"
#include <iostream>

 // if return false i dont have any violation
//  if i return true i have violation
 void safetycheck::VerifyViolation(std::vector<State> valuelist){
	 for(unsigned int i =0;i<valuelist.size();i++){
		 std::vector<double> res;
		 if(private_checklist[i].compare("cart_pos") == 0 ){
			 res = CheckBB(valuelist[i]);
		 }
		 else{
			 res=  this->rs[private_checklist[i]].IsValid(valuelist[i]);
		 }
		 this->distances_from_violation[i] = res;
	 }
	 this->Scoring();
 }

 std::vector<double> safetycheck::CheckBB(State point){
	 std::vector<double> res;
	 for(unsigned int j = 0;j<this->bb.size();j++){
		 // i copy the first 3 values of valuelist[i] (the cartesian position)
		 std::vector<double> cart_pos(point.begin(),point.begin() + 3);
		 // DEBUG
		 bool valid = bb[j].OutBox(cart_pos);
		 if(!valid){
			  res.push_back(0);
		 }else{
			 res.push_back(bb[j].OutBox(cart_pos));
		 }
	 }
	 return res;
}
void safetycheck::Scoring(){
	double score =0;
	double distance = 0;
	int index;
	// compute scoring per each row of distance from violation;
	for(unsigned int j=0;j<distances_from_violation.size();j++){
		std::vector<double>::iterator  it = min_element(distances_from_violation[j].begin(),distances_from_violation[j].end());
		distance = *it;
		index = it - distances_from_violation[j].begin();
		if(private_checklist[j].compare("cart_pos") == 0 ){
			score = (100*distance) / this->bb[index].safe_distance;
			boost::lock_guard<boost::mutex> lock(mutex);
			this->score[j] = (int)score;
		}else{
			if(this->rs[private_checklist[j]].distance_from_mid_point.size()>1){
				score = (100*distance) / this->rs[private_checklist[j]].distance_from_mid_point[index];
				boost::lock_guard<boost::mutex> lock(mutex);
				this->score[j] = (int)score;
			}else{
				score = (100*distance) / this->rs[private_checklist[j]].distance_from_mid_point[0];
				boost::lock_guard<boost::mutex> lock(mutex);
				this->score[j] = (int)score;
			}
		}
	}
}
std::vector<int> safetycheck::GetScore(){
	boost::lock_guard<boost::mutex> lock(mutex);
	return this->score;
}
