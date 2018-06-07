// Variant.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <boost/variant.hpp>


int main()
{
	typedef boost::variant< int, const char*, std::string > my_var_t;
	std::vector< my_var_t > some_values;

	some_values.push_back(10);
	int& a = boost::get<int>(some_values.back());
	std::cout << a << '\n';

	some_values.push_back("Hello there!");
	some_values.push_back(std::string("Wow!"));

	std::string& s = boost::get< std::string >(some_values.back());
	s += " That is great!\n";
	std::cout << s;

	return 0;
}


