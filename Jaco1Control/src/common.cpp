#include "common.hpp"

// global variables
boost::recursive_mutex api_mutex;
boost::atomic<bool>  _first(true);
boost::atomic<bool>  _second(false);
double DEG = arma::datum::pi/180;
double PI  = arma::datum::pi;


// Mathematical routine
void Mat2RPY(arma::mat m,arma::vec & rpy)
{
    double sr,cr;
    //XYZ order
    if (fabs(m.at(2,2)) < arma::datum::eps && fabs(m.at(1,2)) < arma::datum::eps)
    {
        //singularity
        rpy[0] = 0;  // roll is zero
        rpy[1] = atan2(m.at(0,2), m.at(2,2));  // pitch
        rpy[2] = atan2(m.at(1,0), m.at(1,1));  // yaw is sum of roll+yaw
    }
    else
    {
        rpy[0] = atan2(-m.at(1,2), m.at(2,2));        // roll
        // compute sin/cos of roll angle
        sr = sin(rpy[0]);
        cr = cos(rpy[0]);
        rpy[1] = atan2(m.at(0,2), cr * m.at(2,2) - sr * m.at(1,2));  // pitch
        rpy[2] = atan2(-m.at(0,1), m.at(0,0));        // yaw
    }
}
// Auxiliary functions
void  WriteFile(std::vector<State> log,std::string namefile)
{
    std::ofstream myfile(namefile.c_str());
    std::cout << log.empty() << std::endl;
    std::cout << "file size " << log[0].size() << std::endl;
    myfile << log[0].size() << "\n";
    for(unsigned int i =0;i<log.size();i++){
        for(unsigned int j=0;j<log[i].size();j++){
            myfile<<log[i][j]<<" ";
        }
        myfile<<"\n";
    }
    myfile.close();
}
void WriteLogFiles(std::vector<Log> recordedLogs,std::vector<std::string> names){
	for(unsigned int i=0;i<names.size();i++){
		WriteFile(recordedLogs[i],names[i]);
		std::cout << "after write file" <<std::endl;
	}
}

int ReadFile(std::string namefile,std::vector< State > & value){
	std::ifstream infile;
	int task_space_dim;
	try{
	  infile.open(namefile.c_str(),std::ifstream::in);
	  std::string line;
	  // in the first line is defined the taskspacedimension
	  std::getline(infile, line);
	  std::stringstream ss1(line);
	  if ( !(ss1 >> task_space_dim) ){
		  std::cout<<"problem reading number of joint"<< std::endl;
	  }
	  while (std::getline(infile, line)){
			std::stringstream ss(line);
			State app(task_space_dim);
			int index = 0;
			while( !ss.eof() ){
				double ff;
				if ( ss >> ff){
				   app[index] = ff;
				   index++;
				}
			}
			if(!app.empty()){
				value.push_back(app);
			}
	  }
	  infile.close();
	}
	catch (std::ifstream::failure e){
	  std::cerr << "Exception opening/reading/closing file\n";
	}
	return task_space_dim;
};
bool IsOne (int i) {
  return (i==1);
};
std::vector<std::pair<int,int> > ContSeq(std::vector<int> foovec){
	std::vector<int> index;
	std::vector<int>::iterator iter = foovec.begin();
	std::vector<int>::iterator end  = foovec.end();
	std::vector<std::pair<int,int> > seq;
	std::pair<int,int> app;
	// collect all the data
	while((iter = std::find_if(iter,end,IsOne)) != end){
		index.push_back(iter - foovec.begin());
	    iter++;
	}
	// look for contiguous sequence
	int start =  index[0];
	unsigned int i = 1;
    while (i<index.size()){
    	if((index[i] - start ) == 1){
    		// im the contiguous case
    		if((i + 1)< index.size()){
				while((index[i+1] - index[i]) == 1){
					i++;
				}
    		}
    	app.first = start;
    	app.second = index[i];
    	seq.push_back(app);
    	if(i+1 <index.size())
    		start = index[i+1];
    		i = i+2;
    	}else{
    		//update start
    		start = index[i];
    		i++;
    	}
    }
    return seq;
}
// Robot interaction
State convertDirectionToState( std::string direction, float speed){
    State res(3);
    res.zeros(3);
    if(direction.compare( "Right") == 0){
        res[0] = -speed;
    }else if(direction.compare("Down") == 0){
        res[2] = -speed;
    }else if(direction.compare("Left") == 0){
        res[0] = speed;
    }else if(direction.compare("Up") == 0 ){
        res[2] = speed;
    }else if(direction.compare("Push")== 0 ){
        res[1] = -speed;
    }else if(direction.compare("Pull")== 0 ){
        res[1] = speed;
    }else if(direction.compare("Open") == 0){
        res[6] = -30;
        res[7] = -30;
        res[8] = -30;
    }else if(direction.compare("Close") == 0){
        res[6] = 30;
        res[7] = 30;
        res[8] = 30;
    }
    return res;
}

