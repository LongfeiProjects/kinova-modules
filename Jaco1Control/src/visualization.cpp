#include "visualization.hpp"

visualization::visualization()
{}

visualization::visualization(int NBLOCKS,int NJOINTS,int chunk_dim,PLFLT x_min,PLFLT x_max,PLFLT low_thresh,PLFLT high_thresh,std::vector<PLFLT> y_min, std::vector<PLFLT>y_max,
   	        					std::vector<std::string> label,std::vector<std::string> title)
{
	this->NBLOCKS = NBLOCKS;
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
	pls->ssub(1,NBLOCKS);
	cur_size = 1;
    // initialize vector of buffer
	this->time = boost::circular_buffer<PLFLT>(chunk_dim);
	buffer app(NJOINTS);
	for(int i =0;i<NJOINTS;i++)
	{
		app[i] = time;
	}
	for(int i =0;i<NBLOCKS;i++)
	{
		this->buf.push_back(app);
	}

}

// update order: time,joints,torque,current
void visualization::Update(std::vector<std::vector<double>* > & lastval)
{
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


void visualization::Plot()
{
	for(int i = 0; i < this->NBLOCKS;i++)
	{
		pls->adv(i + 1);
		pls->col0( 1 );
		pls->env0(x_min, x_max, y_min[i], y_max[i], 0, 0 );
		pls->col0( 2 );
		pls->lab("time",label[i].c_str(),title[i].c_str());
		pls->col0( 3 );
		for(int j = 0;j<NJOINTS;j++)
			pls->line(cur_size , time.linearize(), buf[i][j].linearize());
	}
}
