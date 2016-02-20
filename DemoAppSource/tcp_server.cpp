#include "tcp_server.h"

#define CONNECTION_TIMEOUT 300000

TCPServer::TCPServer(int portNumber) {
	this->portNumber = portNumber;
	clientFd = 0;
}
int TCPServer::initialize() {
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(!(fd < 0))
	{
		bzero((char *)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(5000);
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		fdFlags = fcntl(fd, F_GETFL, 0);
		fdFlags |= O_NONBLOCK;
		fcntl(fd, F_SETFL, fdFlags);
		if(bind(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
			return -3;
		}
		listen(fd, 5);
		clilen = sizeof(cli_addr);
		return 0;
	}
	return -4;
}
int TCPServer::acceptConnection() 
{
	if(!(clientFd > 0))
	{
		clientFd = accept(fd, (struct sockaddr *) &cli_addr, &clilen);
		if(clientFd < 0)
			return -1;
		clientFdFlags = fcntl(clientFd, F_GETFL, 0);
		clientFdFlags |= O_NONBLOCK;
		fcntl(clientFd, F_SETFL, clientFdFlags);
		timer.start();
		return 0;
	}
	return -1;
}
void TCPServer::restartConnTimeoutTimer() 
{
	if(isConnected() == true)
		timer.restart();
}
bool TCPServer::isConnected() 
{
	if(clientFd > 0)
	{
		if(timer.elapsed() < CONNECTION_TIMEOUT)
		{
			return true;
		}
		else
		{
			releaseClient();
			return false;
		}
	}
	return false;
}
int TCPServer::sendMessage(char *buffer, int numberOfBytes) {
	int count = write(clientFd, buffer, numberOfBytes);
	if(count != numberOfBytes) 
	{
		return -2;
	}
	return 0;
}
int TCPServer::receiveMessage(char *buffer, int numberOfBytes) {
	int count = 0;
	if((count = read(clientFd, (void *) &buffer[0], numberOfBytes)) < 0) 
	{
	}
	else if(count == 0){
	}
	else {
	}
	return count;
}
int TCPServer::release() {
	int ret = close(fd);
	fd = 0;
	return ret;
}
int TCPServer::releaseClient() {
	int ret = close(clientFd);
	clientFd = 0;
	return ret;
}
