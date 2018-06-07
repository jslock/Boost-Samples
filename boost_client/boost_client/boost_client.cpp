// boost_client.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

	// ������ �����ϴ� Resolving ó�� �Լ�
	void handleResolve(const boost::system::error_code& err,
		tcp::resolver::iterator myIterator)
	{
		if (!err)
		{
			// ����Ʈ�� ù ��° endpoint�� ������ �õ�
			// �����ϰ� ������ ������ �� ���� ������ endpoint ���� �õ�
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

	// ������ ������ �����ϴ� �Լ�
	void handleConnect(const boost::system::error_code& err,
		tcp::resolver::iterator myIterator)
	{
		if (!err)
		{
			// ������ �������� ��� ��û�� ������ ��
			boost::asio::async_write(socket, request,
				boost::bind(&tcpClient::handleWriteFile, this,
					boost::asio::placeholders::error));
		}
		else if (myIterator != tcp::resolver::iterator())
		{
			// ���ῡ ������ ��� ����Ʈ�� ���� endpoint�� �õ�
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

	// ������ ������ ���� ���� �Լ�
	void handleWriteFile(const boost::system::error_code& err)
	{
		if (!err)
		{
			// ������ ������ ���� �� ����
			if (sourceFile.eof() == false)
			{
				// ���ۿ� ����
				sourceFile.read(buf.c_array(), (std::streamsize)buf.size());
				if (sourceFile.gcount() <= 0)
				{
					std::cout << "read file error" << std::endl;
					return;
				}

				// ������ ���� ����
				std::cout << "send: " << sourceFile.gcount() << "bytes, total:"
					<< sourceFile.tellg() << "bytes.\n";

				data += sourceFile.gcount();

				// ������ ������ �����ϴ� async_write() �Լ� ����
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
					std::cout << "���� ���� �Ϸ�." << std::endl;
					std::cout << "�� ���۷�: " << data << std::endl;
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

	// tcpClient Ŭ���� ������ ����
	tcpClient(boost::asio::io_service& io_service,
		const std::string& server, const std::string& path)
	: resolver(io_service), socket(io_service)
	{

		data = 0;
		// ������ IP�� ��Ʈ �ּҸ� ������ ����
		size_t pos = server.find(':');
		if (pos == std::string::npos)
			return;
		std::string portString = server.substr(pos + 1);
		std::string serverIP = server.substr(0, pos);

		// �ڽ��� ������ ���� �ҽ� ������ ����
		sourceFile.open(path.c_str(), std::ios_base::binary | std::ios_base::ate);

		// �ҽ� ������ �������� �ʴ� ���
		if (!sourceFile)
		{
			std::cout << "failed to open" << path << std::endl;
			return;
		}

		// ���Ͽ� ���� ������ ��� ����
		size_t fileSize = (size_t)sourceFile.tellg();
		sourceFile.seekg(0);

		// ���� ������ ������ �̸��� ������ ũ�⸦ ����
		std::ostream requestStream(&request);
		requestStream << path << "\n" << fileSize << "\n\n";
		std::cout << "request size : " << request.size() << std::endl;

		// ������ ������ �̸��� �����ϱ� ���� �񵿱������ resolving ó��
		// endpoints�� ����Ʈ ó��
		tcp::resolver::query query(serverIP, portString);
		resolver.async_resolve(query, boost::bind(&tcpClient::handleResolve, this,
			boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	}
};

int main(int argc, char* argv[])
{
	try
	{
		// tcpClient ��ü�� �����ؼ� Ŭ���̾�Ʈ ���α׷� ����
		boost::asio::io_service io_service;
		tcpClient client(io_service, "127.0.0.1:39800", "c:\\Temp\\a.txt");
		io_service.run();

		// ������ ���� �ڿ� ���� ���� ���� ���
		std::cout << "send file " << "c:\\Temp\\a.txt" << " completed successfully.\n";
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	system("pause");
	return 0;
}
