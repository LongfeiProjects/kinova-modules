#include "visualization.hpp"

visualization::visualization()
{}

visualization::visualization(int NJOINTS,int chunk_dim,PLFLT x_min,PLFLT x_max,PLFLT low_thresh,PLFLT high_thresh,std::vector<PLFLT> y_min, std::vector<PLFLT>y_max,
   	        					std::vector<std::string> label,std::vector<std::string> title)
{
	this->NJOINTS = NJOINTS;
	this->chunk_dim = chunk_dim;
	this->x_min = x_min;
	this->x_max = x_max;
	this->low_thresh = low_thresh;
	this->high_thresh= high_thresh;
	this->y_min = y_min;
	this->y_max = y_max;
	this->label = label;
	this->title = title;
	// initialization strema
	pls = new plstream();
    pls->sdev("wxwidgets");
	pls->spage(0,0,800,1500,0,0);
	pls->setopt("db", "");
	pls->setopt("np", "");
	pls->init();
	pls->ssub(1,3);
}





void visualization::Update(std::vector<std::vector<double>* > lastval)
{
	// update order: time,joints,torque,current

	// update time
	time.push_back(lastval[0]->at(0));
	// update the other value
	for(int i=0;i<buf.size();i++)
	{
		for(int j=0;j<NJOINTS;j++)
			buf[i][j].push_back(lastval[i]->at(j));
	}

	if(time.front()-x_min>low_thresh)
	{
		x_min = time.front();
	}
	if(x_max-time.back()<high_thresh)
		x_max = x_max+high_thresh;

	cur_size++;
}


void visualization::Plot(int blocks)
{
	pls->adv(blocks + 1);
	pls->col0( 1 );
	pls->env0(x_min, x_max, y_min[blocks], y_max[blocks], 0, 0 );
	pls->col0( 2 );
	pls->lab("time",label[blocks].c_str(),title[blocks].c_str());
	pls->col0( 3 );
	for(int i = 0;i<NJOINTS;i++)
	pls->line(cur_size , time.linearize(), buf[blocks][i].linearize());
}
