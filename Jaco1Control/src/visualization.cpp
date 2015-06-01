#include "visualization.hpp"

visualization::visualization()
{}
visualization::visualization(int NBLOCKS,int NJOINTS,int chunk_dim,PLFLT x_min,PLFLT x_max,PLFLT low_thresh,PLFLT high_thresh,const double y_min[3],
		                        const double y_max[],const char * label[],const char * title[])
: data(NBLOCKS,NJOINTS,chunk_dim,x_min,x_max,low_thresh,high_thresh,y_min,y_max,label,title)
{
	// initialization strema
	pls = new plstream();
    pls->sdev("wxwidgets");
	pls->spage(0,0,800,900,0,0);
	pls->setopt("db", "");
	pls->setopt("np", "");
	pls->init();
	pls->ssub(1,data.NBLOCKS);
	cur_size = 0;
    // initialize vector of buffer
	this->time = boost::circular_buffer<PLFLT>(data.chunk_dim);
	buffer app(data.NJOINTS);
	for(int i =0;i<data.NJOINTS;i++)
	{
		app[i] = time;
	}
	for(int i =0;i<data.NBLOCKS;i++)
	{
		this->buf.push_back(app);
	}

}

visualization::visualization(data_vis data)
{
	this->data = data;
	// initialization strema
	pls = new plstream();
	pls->sdev("wxwidgets");
	pls->spage(0,0,800,900,0,0);
	pls->setopt("db", "");
	pls->setopt("np", "");
	pls->init();
	pls->ssub(1,data.NBLOCKS);
	cur_size = 0;
	// initialize vector of buffer
	this->time = boost::circular_buffer<PLFLT>(data.chunk_dim);
	buffer app(data.NJOINTS);
	for(int i =0;i<data.NJOINTS;i++)
	{
		app[i] = time;
	}
	for(int i =0;i<data.NBLOCKS;i++)
	{
		this->buf.push_back(app);
	}

}

// update order: time,joints,torque,current
void visualization::Update(std::vector<std::vector<double>* > & lastval)
{
	//DEBUG
	//std::cout<<"lastval.size = "<<lastval.size()<<std::endl;
	//std::cout<<"buf.size = "<<buf.size()<<std::endl;
	//std::cout<< "update time in Update" << std::endl;
	//---
	// update time
	time.push_back(lastval[0]->at(0));

	//DEBUG
	//std::cout<< "update other variables in Update" << std::endl;
	//---
	// update the other value
	for(unsigned int i=0;i<buf.size();i++)
	{
		//DEBUG
		//std::cout<<"current buf size = "<< buf[i].size()<<std::endl;
		//---

		for(int j=0;j<data.NJOINTS;j++)
			buf[i][j].push_back(lastval[i + 1]->at(j));
	}

	// dynamic update of the graphs
	if(time.front()-data.x_min>data.low_thresh)
	{
		data.x_min = time.front();
	}
	if(data.x_max-time.back()<data.high_thresh)
		data.x_max = data.x_max+data.high_thresh;

	if(this->cur_size<data.chunk_dim)
		cur_size++;
}


void visualization::Plot()
{
	for(int i = 0; i < this->data.NBLOCKS;i++)
	{
		pls->adv(i + 1);
		pls->col0( 1 );
		pls->env0(data.x_min, data.x_max, data.y_min[i], data.y_max[i], 0, 0 );
		pls->col0( 2 );
		pls->lab("time",data.label[i].c_str(),data.title[i].c_str());
		for(int j = 0;j<data.NJOINTS;j++)
		{
			pls->col0( 3+j );
			pls->line(cur_size , time.linearize(), buf[i][j].linearize());
		}

	}
}
