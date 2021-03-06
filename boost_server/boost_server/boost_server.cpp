// boost_server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

unsigned short port = 39800;

// 실질적인 통신을 담당하는 tcpConnection 클래스 정의*
class tcpConnection : public
	boost::enable_shared_from_this <tcpConnection>
{
private:
	boost::asio::streambuf requestBuf;
	boost::asio::ip::tcp::socket mySocket;
	boost::array<char, 40960> buf;
	size_t fileSize;
	std::ofstream outputFile;

	// 파일을 읽어오는 요청 함수 다루기
	void handleReadRequest(const boost::system::error_code& err,
		std::size_t bytesTransferred)
	{

		if (err)
		{
			return handleError(__FUNCTION__, err);
		}

		// 오류가 발생하지 않은 경우 파일 다운의 기본 설정 출력
		std::cout << __FUNCTION__ << "(" << bytesTransferred << ")"
			<< ", in_avail = " << requestBuf.in_avail()
			<< ", size = " << requestBuf.size()
			<< ", max_size = " << requestBuf.max_size()
			<< ".\n";

		// 파일에 대한 정보를 담을 변수
		std::istream requestStream(&requestBuf);
		std::string filePath;
		requestStream >> filePath;
		requestStream >> fileSize;

		// 앞의 개행 문자 2개 제거
		requestStream.read(buf.c_array(), 2);

		// 파일에 대한 정보 출력
		std::cout << filePath << " size is " << fileSize << ", tellg = " <<
			requestStream.tellg() << std::endl;
		size_t pos = filePath.find_last_of('\\');
		if (pos != std::string::npos)
			filePath = filePath.substr(pos + 1);

		// 이제 내 컴퓨터에 출력할 파일을 열음
		outputFile.open(filePath.c_str(), std::ios_base::binary);

		// 출력 파일이 존재하지 않는 경우
		if (!outputFile)
		{
			std::cout << "failed to open" << filePath << std::endl;
			return;
		}

		// 파일의 여분 바이트를 써내려감
		do
		{
			requestStream.read(buf.c_array(), (std::streamsize) buf.size());
			std::cout << __FUNCTION__ << " write " << requestStream.gcount() << "bytes.\n";
			outputFile.write(buf.c_array(), requestStream.gcount());
			
		} while (requestStream.gcount() > 0);
			
		// 버퍼에 정보를 읽어 옴
		async_read(mySocket, boost::asio::buffer(buf.c_array(), buf.size()),
			boost::bind(&tcpConnection::handleReadFileContent,
				shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	// 파일의 내용을 읽는 기능
	void handleReadFileContent(const boost::system::error_code& err,
		std::size_t bytesTransferred)
	{
		// 바이트 전송
		if (bytesTransferred > 0)
		{
			outputFile.write(buf.c_array(), (std::streamsize) bytesTransferred);
			std::cout << __FUNCTION__ << "recv" << outputFile.tellp() << "bytes." << std::endl;
			if (outputFile.tellp() >= (std::streamsize) fileSize)
			{
				return;
			}
		}

		if (err)
		{
			return handleError(__FUNCTION__, err);
		}

		// 버퍼에 정보를 읽어 옴
		async_read(mySocket, boost::asio::buffer(buf.c_array(), buf.size()),
			boost::bind(&tcpConnection::handleReadFileContent,
				shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	// 오류 처리 함수
	void handleError(const std::string& functionName, const
		boost::system::error_code& err)
	{
		std::cout << __FUNCTION__ << " in " << functionName << " due to " << err
			<< " " << err.message() << std::endl;
	}
public:

	// tcpConnection 클래스의 생성자
	tcpConnection(boost::asio::io_service& io_service)
		:mySocket(io_service), fileSize(0)
	{}

	// 실행 함수
	void start()
	{
		std::cout << __FUNCTION__ << std::endl;
		async_read_until(mySocket, requestBuf, "\n\n",
			boost::bind(&tcpConnection::handleReadRequest,
				shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	// 현재의 소켓을 반환하는 함수
	boost::asio::ip::tcp::socket& socket()
	{
		return mySocket;
	}
};

// tcpServer라는 서버 클래스를 정의
class tcpServer : private boost::noncopyable
{

	// 클래스 운영에 필요한 기본 변수 정의
private:
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::acceptor acceptor;
public:

	// tcpConnection 클래스의 포인터 변수 선언
	typedef boost::shared_ptr <tcpConnection> ptrTcpConnection;

	// 서버 생성자
	tcpServer(unsigned short port)
	: acceptor(ioService,
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), true)
	{

		// 서버 실행
		ptrTcpConnection newConnection(new tcpConnection(ioService));
		acceptor.async_accept(newConnection->socket(),
			boost::bind(&tcpServer::handleAccept, this, newConnection,
				boost::asio::placeholders::error));
		ioService.run();
	}

	void handleAccept(ptrTcpConnection currentConnection, const
		boost::system::error_code& e)
	{
		std::cout << __FUNCTION__ << " " << e << ", " << e.message() << std::endl;
		if (!e)
		{
			currentConnection->start();
		}
	}
	~tcpServer()
	{
		ioService.stop();
	}
};

// 메인 함수
int main(int argc, char *argv[])
{
	try
	{
		if (argc == 2)
		{
			port = atoi(argv[1]);
		}
		std::cout << argv[0] << " listen on port " << port << std::endl;
		tcpServer *myTcpServer = new tcpServer(port);
		delete myTcpServer;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	// 프로그램 종료
	system("pause");
	return 0;
}
