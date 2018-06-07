// boost_client.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

class tcpClient
{
private:

	tcp::resolver resolver;
	tcp::socket socket;
	boost::array<char, 1024> buf;
	boost::asio::streambuf request;
	std::ifstream sourceFile;
	size_t data;

	// 서버에 세팅하는 Resolving 처리 함수
	void handleResolve(const boost::system::error_code& err,
		tcp::resolver::iterator myIterator)
	{
		if (!err)
		{
			// 리스트의 첫 번째 endpoint로 연결을 시도
			// 완전하게 연결이 성립될 때 까지 각각의 endpoint 연결 시도
			tcp::endpoint endpoint = *myIterator;
			socket.async_connect(endpoint,
				boost::bind(&tcpClient::handleConnect, this,
					boost::asio::placeholders::error, ++myIterator));
		}
		else
		{
			std::cout << "Error :: " << err.message() << "\n";
		}
	}

	// 실제로 연결을 실행하는 함수
	void handleConnect(const boost::system::error_code& err,
		tcp::resolver::iterator myIterator)
	{
		if (!err)
		{
			// 연결이 성공적인 경우 요청을 보내게 됨
			boost::asio::async_write(socket, request,
				boost::bind(&tcpClient::handleWriteFile, this,
					boost::asio::placeholders::error));
		}
		else if (myIterator != tcp::resolver::iterator())
		{
			// 연결에 실패한 경우 리스트의 다음 endpoint로 시도
			socket.close();
			tcp::endpoint endpoint = *myIterator;
			socket.async_connect(endpoint,
				boost::bind(&tcpClient::handleConnect, this,
					boost::asio::placeholders::error, ++myIterator));
		}
		else
		{
			std::cout << "Error : " << err.message() << "\n";
		}
	}

	// 실제로 파일을 쓰기 위한 함수
	void handleWriteFile(const boost::system::error_code& err)
	{
		if (!err)
		{
			// 보내는 파일이 끝날 때 까지
			if (sourceFile.eof() == false)
			{
				// 버퍼에 담음
				sourceFile.read(buf.c_array(), (std::streamsize)buf.size());
				if (sourceFile.gcount() <= 0)
				{
					std::cout << "read file error" << std::endl;
					return;
				}

				// 보내는 파일 정보
				std::cout << "send: " << sourceFile.gcount() << "bytes, total:"
					<< sourceFile.tellg() << "bytes.\n";

				data += sourceFile.gcount();

				// 실제로 파일을 전송하는 async_write() 함수 실행
				boost::asio::async_write(socket,
					boost::asio::buffer(buf.c_array(), (size_t)sourceFile.gcount()),
					boost::bind(&tcpClient::handleWriteFile, this,
						boost::asio::placeholders::error));

				if (err)
				{
					std::cout << "send error : " << err << std::endl;
					return;
				}
				else
				{
					std::cout << "파일 전송 완료." << std::endl;
					std::cout << "총 전송량: " << data << std::endl;
					sourceFile.close();
					return;
				}
			}
			else
				return;
		}
		else
		{
			std::cout << "Error : " << err.message() << "\n";
		}
		
	}
public:

	// tcpClient 클래스 생성자 정의
	tcpClient(boost::asio::io_service& io_service,
		const std::string& server, const std::string& path)
	: resolver(io_service), socket(io_service)
	{

		data = 0;
		// 서버의 IP와 포트 주소를 나누어 저장
		size_t pos = server.find(':');
		if (pos == std::string::npos)
			return;
		std::string portString = server.substr(pos + 1);
		std::string serverIP = server.substr(0, pos);

		// 자신의 보내고 싶은 소스 파일을 열음
		sourceFile.open(path.c_str(), std::ios_base::binary | std::ios_base::ate);

		// 소스 파일이 존재하지 않는 경우
		if (!sourceFile)
		{
			std::cout << "failed to open" << path << std::endl;
			return;
		}

		// 파일에 대한 정보를 담는 변수
		size_t fileSize = (size_t)sourceFile.tellg();
		sourceFile.seekg(0);

		// 먼저 서버에 파일의 이름과 파일의 크기를 전송
		std::ostream requestStream(&request);
		requestStream << path << "\n" << fileSize << "\n\n";
		std::cout << "request size : " << request.size() << std::endl;

		// 서버와 서비스의 이름을 번역하기 위해 비동기식으로 resolving 처리
		// endpoints의 리스트 처리
		tcp::resolver::query query(serverIP, portString);
		resolver.async_resolve(query, boost::bind(&tcpClient::handleResolve, this,
			boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	}
};

int main(int argc, char* argv[])
{
	try
	{
		// tcpClient 객체를 생성해서 클라이언트 프로그램 실행
		boost::asio::io_service io_service;
		tcpClient client(io_service, "127.0.0.1:39800", "c:\\Temp\\a.txt");
		io_service.run();

		// 파일을 보낸 뒤에 보낸 파일 정보 출력
		std::cout << "send file " << "c:\\Temp\\a.txt" << " completed successfully.\n";
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	system("pause");
	return 0;
}
