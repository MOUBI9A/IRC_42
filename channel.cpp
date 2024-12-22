#include "channel.hpp"

Channel::Channel()
{
    this->channelName = "";
    this->hasPassword = false;
    this->hasTopic = true;
    this->hasLimit = false;
    this->inviteOnly = false;
    this->limit = MAX_CLIENTS;
    this->password = "";
    this->topic = "";
    this->userCount = 0;
    this->access = nullptr;
    this->creationTime = std::time(nullptr);
}

Channel::Channel(std::string channelName, std::string password, std::string topic)
{
    this->channelName = channelName;
    this->hasPassword = !password.empty();
    this->password = password;
    this->hasTopic = true;
    this->topic = topic;
    this->userCount = 0;
    this->access = nullptr;
    this->hasLimit = false;
    this->inviteOnly = false;
    this->limit = MAX_CLIENTS;
}

Channel::~Channel()
{
}

void Channel::setChannelName(std::string channelName)
{
    this->channelName = channelName;
}
std::string Channel::getChannelName()
{
    return this->channelName;
}

void Channel::setHasPassword(bool hasPassword)
{
    this->hasPassword = hasPassword;
}
bool Channel::getHasPassword()
{
    return this->hasPassword;
}

void Channel::setPassword(std::string password)
{
    this->password = password;
}
std::string Channel::getPassword()
{
    return this->password;
}

void Channel::setHasTopic(bool hasTopic)
{
    this->hasTopic = hasTopic;
}
bool Channel::getHasTopic()
{
    return this->hasTopic;
}

void Channel::setTopic(std::string topic)
{
    this->topic = topic;
}
std::string Channel::getTopic()
{
    return this->topic;
}

void Channel::setAccess(t_ch_access *access)
{
    this->access = access;
}
t_ch_access *Channel::getAccess()
{
    return this->access;
}

void Channel::setUserCount(int userCount)
{
    this->userCount = userCount;
}
int Channel::getUserCount()
{
    return this->userCount;
}

void Channel::addClient(Client cl)
{
    t_ch_access *newAccess = new t_ch_access;
    newAccess->cl = cl;
    newAccess->next = this->access;
    this->access = newAccess;
    this->userCount++;
}

void Channel::removeClient(Client cl)
{
    t_ch_access *tmp = this->access;
    t_ch_access *prev = nullptr;
    while (tmp != nullptr)
    {
        if (tmp->cl.getNickName() == cl.getNickName())
        {
            if (prev == nullptr)
                this->access = tmp->next;
            else
                prev->next = tmp->next;
            delete tmp;
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
}

void Channel::setPermission(Client cl,bool isOperator)
{
    t_ch_access *tmp = this->access;
    while (tmp != nullptr)
    {
        if (tmp->cl.getNickName() == cl.getNickName())
        {
            tmp->isOperator = isOperator;
            return;
        }
        tmp = tmp->next;
    }
}

bool Channel::getPermission(Client cl, std::string permission)
{
    t_ch_access *tmp = this->access;
    while (tmp != nullptr)
    {
        if (tmp->cl.getNickName() == cl.getNickName())
        {
            if (permission == "operator")
                return tmp->isOperator;
            
        }
        tmp = tmp->next;
    }
    return false;
}

std::vector<Client> Channel::getClients()
{
    std::vector<Client> clients;
    t_ch_access *tmp = this->access;
    while (tmp != nullptr)
    {
        clients.push_back(tmp->cl);
        if (tmp->isOperator)
            clients.back().setOperator(true);
        tmp = tmp->next;
    }
    return clients;
}


void Channel::setCreationTime(std::time_t creationTime)
{
    this->creationTime = creationTime;
}

std::time_t Channel::getCreationTime()
{
    return this->creationTime;
}


std::string Channel::getModes()
{
    std::string modes = "+";
    modes += this->hasTopic ? "t" : "";
    modes += this->inviteOnly ? "i" : "";
    modes += this->hasPassword ? "k" : "";
    modes += this->hasLimit ? "l" : "";

    modes += this->hasPassword ? " " + this->password : "";
    modes += this->hasLimit ? " " + std::to_string(this->limit) : "";

    return modes;
}


void Channel::setLimit(int limit)
{
    this->limit = limit;
}

int Channel::getLimit()
{
    return this->limit;
}

void Channel::setInviteOnly(bool inviteOnly)
{
    this->inviteOnly = inviteOnly;
}

bool Channel::getInviteOnly()
{
    return this->inviteOnly;
}

void Channel::setHasLimit(bool hasLimit)
{
    this->hasLimit = hasLimit;
}

bool Channel::getHasLimit()
{
    return this->hasLimit;
}
