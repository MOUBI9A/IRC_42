#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "client.hpp"
#include <ctime> // For std::time_t
#include <string> // For std::string

#define MAX_CLIENTS 10


typedef struct s_ch_access
{
    Client cl;
    bool isOperator;

    struct s_ch_access *next;
} t_ch_access;

class Channel 
{
    private:
        std::string channelName;
        bool hasPassword;
        bool hasTopic;
        bool hasLimit;
        bool inviteOnly;
        int limit;
        std::string password;
        std::string topic;
        std::time_t creationTime;
        std::vector<Client> InvitedClients;

        int userCount;
        

        t_ch_access *access;



    public:
        Channel();
        Channel(std::string channelName, std::string password, std::string topic);
        ~Channel();

        // void setInvitedClients(std::vector<Client> InvitedClients);

        // std::vector<Client> getInvitedClients();
        void addInvitedClient(Client cl);
        void removeInvitedClient(Client cl);
        bool isInvited(Client cl);

        void setChannelName(std::string channelName);
        std::string getChannelName();

        void setHasPassword(bool hasPassword);
        bool getHasPassword();

        void setPassword(std::string password);
        std::string getPassword();

        void setHasTopic(bool hasTopic);
        bool getHasTopic();

        void setTopic(std::string topic);
        std::string getTopic();

        void setUserCount(int userCount);
        int getUserCount();

        void setAccess(t_ch_access *access);
        t_ch_access *getAccess();

        void setCreationTime(std::time_t creationTime);
        std::time_t getCreationTime();

        void setLimit(int limit);
        int getLimit();

        void setInviteOnly(bool inviteOnly);
        bool getInviteOnly();

        void setHasLimit(bool hasLimit);
        bool getHasLimit();

        void setPermission(Client cl,bool isOperator);
        bool getPermission(Client cl, std::string permission);
        
        std::vector<Client> getClients();


        void addClient(Client cl);
        void removeClient(Client cl);

        


        std::string getModes();




        


};




#endif // Channel_HPP
