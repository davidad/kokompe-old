#pragma once
#include <set>

class application;

#include "design.h"
#include "view.h"

class application
{
	private:
		std::set<design*> open_designs;
		std::set<view*> open_views;
		design* new_design();
		void new_view(design& d);
		bool time_to_shutdown;
	public:
		application(int argc, char** argv);
		~application();
		void run();
		void new_view();
		void initiate_shutdown();
};
