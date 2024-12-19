#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>                                        
#include <sys/socket.h>                                 
#include <sys/types.h>                                
#include <netinet/in.h>                             
#include <fcntl.h>                                
#include <unistd.h>                                 
#include <arpa/inet.h>                            
#include <poll.h>                               
#include <csignal>     

#include "server.hpp"
#include <sstream> 





#define RED "\e[1;31m"
#define WHI "\e[0;37m" 
#define GRE "\e[1;32m"
#define YEL "\e[1;33m" 
#define ORNG "\033[33m"
#define BLUE "\033[34m"

#include "client.hpp" // Include the Client header
#include "channel.hpp" // Include the Channel header

class Client;
class Channel;


#define MAX_CHANNEL 10

// REPLIES AND ERRORS 

#define RPL_WELCOME(client, servername)														("001 " + client + " :Welcome to" + servername + "the most amazing IRC server u can use it to sell drugs, buy guns and do whatever you want\r\n")

#define ERR_NOSUCHCHANNEL(client, channel)	                                    ("403 " + client + " " + channel + " :No such channel\r\n")

#define ERR_NICKNAMEINUSE(client, nick)		                                    ("433 " + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_NOTREGISTERED(client)	                                      		("451 " + client + " JOIN :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(client, command)	                                    ("461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_PASSWDMISMATCH(client)			                                    ("464 " + client + " :Password incorrect\r\n")
#define ERR_CHANNELISFULL(client, channel)                                     	("471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_TOOMANYCHANNELS(client, channel) 									("405 " + client + " " + channel + " :You have joined too many channels\r\n")
#define ERR_BADCHANNELKEY(client, channel) 										("475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_BANNEDFROMCHAN(client, channel) 									("474 " + client + " " + channel + " :Cannot join channel (+b)\r\n")
#define ERR_CHANNELISFULL(client, channel) 										("471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(client, channel) 									("473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANMASK(channel) 												("476 " + channel + " :Bad Channel Mask\r\n")
#define ERR_NOTONCHANNEL(client, channel) 										("442 " + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) 									("482 " + client + " " + channel + " :You're not channel operator\r\n")
#define ERR_USERNOTINCHANNEL(client, nick, channel) 							("441" + client + " " + nick + " " + channel +  " :They aren't on that channel\r\n")
#define ERR_USERONCHANNEL(client, nick, channel) 								("443 " + client + " " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_INVITEONLYCHAN(client, channel) 									("473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define	ERR_KEYSET(client, channel)												("467 " + client + " " + channel + " :Channel key already set\r\n")
#define RPL_NOTOPIC(client, channel) 											("331 " + client + " " + channel + " :No topic is set\r\n")
#define RPL_TOPIC(client, channel, topic) 										("332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_TOPICWHOTIME(client, channel, nick, setat) 							("333 " + client + " " + channel + " " + nick + " " + setat + "\r\n")
#define RPL_NAMREPLY(prefix, nick) 												(prefix + nick + " ")
#define RPL_ENDOFNAMES(client, channel) 										("366 " + client + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_INVITING(client, nick, channel) 									("341 " + client + " " + nick + " " + channel + "\r\n")
// #define ERR_UNKNOWNMODE(client, modechar)										("472 " + client + " " + modechar + " :is unknown mode char to me\r\n")
#define ERR_NORECIPIENT(client, command)										("411 " + client + " " + command + " :No recipient given (" + command + ")\r\n")
#define RPL_ENDOFWHO(client, mask) 												("315 " + client + " " + mask + " :End of WHO list.\r\n")
#define RPL_WHOREPLY(client, channel, username, host, server, nick, prefixes, realname)	("352 " + client + " " + channel + " " + username + " " + host + " " + server + " " + nick + " H" + prefixes + " :0 " + realname + "\r\n")

#define RPL_CHANNELMODEIS(client, channel, modes)								("324 " + client + " " + channel + " " + modes + "\r\n")
#define	ERR_NOSUCHNICK(client, nickname)	("401 " + client + " " + nickname + " :No such nick\r\n")
#define ERR_NOTEXTTOSEND(client)			("412 " + client + " :No text to send\r\n")
#define RPL_CREATIONTIME(client, channel, time)	("329 " + client + " " + channel + " " + time + "\r\n")

class Server
{
    private:
        std::string serverName;
        static bool Signal;
        int fd_soket;
        std::vector<pollfd>fds;

        std::vector<Client> clients; // Use the Client type
        std::vector<Channel> channels; // Use the Channel type
        
        int port;
        std::string password;

        int chanelCount;




    public :

        Server();
        ~Server();

        void setServerName(std::string serverName);
        std::string getServerName();


        static void SignalHandler(int signum);
        void ServerInit(int port, std::string password);
        void Sersocket();
        void closeFDS();
        void AcceptNewclient();
        void ReceiveNewData(int fd);
        void Clearclients(int fd);



        // for time 

        std::string getCurrentTime();
        void Msg_resv();

        // exucute commands
        void executeCommand(int fd, char *buffer);
        void findCommand(int fd, std::string command, std::string param);
        bool findChanel(std::string channelName);
        bool findUserNick(std::string nickName);

        Client getUserbyNick(std::string nick);
        Client* getUserbyFD(int fd);

        Channel getChannelbyName(std::string channelName);


        void Createchanel(std::string channelName, std::string password, std::string topic);
        void addClientToChannel(Client cl, std::string channelName);
        void send_to_members(std::string channelName, std::string msg, int fd);
        
        
};

#endif // SERVER_HPP