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
	 //DEBUG
	 /*std::cout<< "distances_from_violation"<<std::endl;
	 for(unsigned int i = 0;i<distances_from_violation.size();i++){
		 for(unsigned int j = 0;j<distances_from_violation[i].size();j++)
			 std::cout<< distances_from_violation[i][j]<<" ";
		 std::cout<<std::endl;
	 }*/
	 this->Scoring();
	 //DEBUG
	 /*std::cout<<"score"<<std::endl;
	 for(unsigned int j = 0;j<score.size();j++)
		 std::cout<< score[j]<<" ";
	 std::cout<<std::endl;*/
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
			 res.push_back(bb[j].DistBox(cart_pos));
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
		//DEBUG
		//std::cout << private_checklist[j]<<" min dist = "<<distance<<" ";
		index = it - distances_from_violation[j].begin();
		if(private_checklist[j].compare("cart_pos") == 0 ){
			score = (100*distance) / this->bb[index].safe_distance;
			if(score>100)
				score = 100;
			// invert score
			score = 100 - score;
			boost::lock_guard<boost::mutex> lock(mutex);
			this->score[j] = (int)score;
		}else{
			if(this->rs[private_checklist[j]].distance_from_mid_point.size()>1){
				score = (100*distance) / this->rs[private_checklist[j]].distance_from_mid_point[index];
				if(score>100)
					score = 100;
				// invert score
				score = 100 - score;
				boost::lock_guard<boost::mutex> lock(mutex);
				this->score[j] = (int)score;
			}else{
				score = (100*distance) / this->rs[private_checklist[j]].distance_from_mid_point[0];
				if(score>100)
					score = 100;
				// invert score;
				score = 100 - score;
				boost::lock_guard<boost::mutex> lock(mutex);
				this->score[j] = (int)score;
			}
		}
	}
	//DEBUG
	//std::cout<<std::endl;
}
std::vector<int> safetycheck::GetScore(){
	boost::lock_guard<boost::mutex> lock(mutex);
	return this->score;
}
