// ProgramOptions.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <boost/program_options.hpp>


namespace opt = boost::program_options;


int main(int argc, char** argv)
{

	opt::options_description desc("All options");
	desc.add_options()
		("apples", opt::value<int>(), "how many apples do you have")
		("oranges", opt::value<int>(), "how many oranges do you have")
		("help", "produce help message");

	opt::variables_map vm;
	opt::store(opt::parse_command_line(argc, argv, desc), vm);
	opt::notify(vm);

	if (vm.size() == 0 || vm.count("help"))
	{
		std::cout << desc << std::endl;
	}
	else
	{
		std::cout << "Fruits count: "
			<< vm["apples"].as<int>() + vm["oranges"].as<int>() << std::endl;
	}

	return 0;
}