#include "server.hpp"

#include "client.hpp"




void Client::setFD(int fd)
{
    this->FD = fd;
}
int Client::getFD()
{
    return this->FD;
}

void Client::setIPadd(std::string ipadd)
{
    this->addIP = ipadd;
}
std::string Client::getIPadd()
{
    return this->addIP;
}

void Client::setUserName(std::string username)
{
    this->UserName = username;
}
std::string Client::getUserName()
{
    return this->UserName;
}

void Client::setNickName(std::string nickname)
{
    
    this->NickName = nickname;
}
std::string Client::getNickName()
{
    return this->NickName;
}

void Client::setRealName(std::string realname)
{
    this->RealName = realname;
}
std::string Client::getRealName()
{
    return this->RealName;
}

void Client::setPass(std::string pass)
{
    this->Pass = pass;
}
std::string Client::getPass()
{
    return this->Pass;
}

void Client::setSigned(bool sign)
{
    this->isSigned = sign;
}
bool Client::getSigned()
{
    return this->isSigned;
}

void Client::setOperator(bool op)
{
    this->isOperator = op;
}

bool Client::getOperator()
{
    return this->isOperator;
}

Client::Client()
{
    this->FD = -1;
    this->UserName = "";
    this->NickName = "";
    this->RealName = "* :realname";
    this->Pass = "";
    this->isSigned = false;
    this->isOperator = false;
}
Client::~Client(){}

Client &Client::operator=(const Client &cl)
{
    this->FD = cl.FD;
    this->UserName = cl.UserName;
    this->NickName = cl.NickName;
    this->RealName = cl.RealName;
    this->Pass = cl.Pass;
    this->isSigned = cl.isSigned;
    this->isOperator = cl.isOperator;
    return *this;
}
