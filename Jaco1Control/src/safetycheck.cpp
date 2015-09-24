#include "safetycheck.hpp"
#include <iostream>

 // if return false i dont have any violation
//  if i return true i have violation
 bool safetycheck::VerifyViolation(std::vector<State> valuelist)
 {
	 bool valid;
	 // if one of the control fail i return true
	 for(unsigned int i =0;i<valuelist.size();i++)
	 {
		 if(checklist[i].compare("cart_pos") == 0 )
		 {
			 for(unsigned int j = 0;j<this->bb.size();j++)
			 {
				 // i copy the first 3 values of valuelist[i] (the cartesian position)
				 std::vector<double> cart_pos(valuelist[i].begin(),valuelist[i].begin() + 3);
				 // DEBUG
				 valid = bb[j].OutBox(cart_pos);
				 if(!valid)
				 {
					 std::cout<< "bounding box violation"<< std::endl;
					 return true;
				 }
			 }
		 }
		 else
		 {
			 valid = this->rs[checklist[i]].IsValid(valuelist[i]);
			 if(!valid)
			 {
				 std::cout<< "violated "<< checklist[i] << std::endl;
				 return true;
			 }
		 }


	 }

	 return false;

 }
