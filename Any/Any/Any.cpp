// Any.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
	// #1: boost::bad_any_cast 예외를 던질 수 있다.
	std::string s1 = boost::any_cast< std::string >(variable);

	// #2: 변수의 실제값이 std::string이 아니라면 nullptr를 반환한다.
	std::string* s2 = boost::any_cast< std::string >(&variable);

	std::cout << *s2 << '\n';

	return 0;
}
