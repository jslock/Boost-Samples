// Any.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <boost/any.hpp>

int main()
{
	/*
	std::vector<boost::any> some_values;

	some_values.push_back(10);
	const char* c_str = "Hello there!";
	some_values.push_back(c_str);
	some_values.push_back(std::string("Wow!"));
	std::string& s = boost::any_cast< std::string& >(some_values.back());
	s += " That is great!\n";
	std::cout << s;
	*/

	boost::any variable(std::string("Hello world!"));

	//variable = 2;
	// #1: boost::bad_any_cast ���ܸ� ���� �� �ִ�.
	std::string s1 = boost::any_cast< std::string >(variable);

	// #2: ������ �������� std::string�� �ƴ϶�� nullptr�� ��ȯ�Ѵ�.
	std::string* s2 = boost::any_cast< std::string >(&variable);

	std::cout << *s2 << '\n';

	return 0;
}
