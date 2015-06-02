#include "safetycheck.hpp"
#include <iostream>

 // if return false i dont have any violation
//  if i return true i have violation
 bool safetycheck::VerifyViolation(std::vector< std::vector<double > > valuelist, std::vector<std::string> list_of_ckeck)
 {
	 bool valid;
	 // if one of the control fail i return true
	 for(unsigned int i =0;i<valuelist.size();i++)
	 {
		 if(list_of_ckeck[i].compare("BoundingBox"))
		 {
			 for(unsigned int j = 0;j<this->bb.size();j++)
			 {
				 valid = bb[j].OutBox(valuelist[i]);
				 if(!valid)
				 {
					 std::cout<< "bounding box violation"<< std::endl;
					 return true;
				 }
			 }
		 }
		 else
		 {
			 valid = this->rs[list_of_ckeck[i]].IsValid(valuelist[i]);
			 if(!valid)
			 {
				 std::cout<< "violated "<< list_of_ckeck[i] << std::endl;
				 return true;
			 }
		 }


	 }

	 return false;

 }
