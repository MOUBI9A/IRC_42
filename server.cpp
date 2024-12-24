#include "server.hpp"

bool Server::Signal = false;

void Server::Createchanel(std::string channelName, std::string password, std::string topic)
{
    Channel newChannel(channelName, password, topic);
    channels.push_back(newChannel);
    this->chanelCount++;   
}


Server::Server()
{
    this->fd_soket = -1;
    clients.clear();
    fds.clear();
    this->port = 8080;
    this->password = "";
    channels.clear();
    chanelCount = 0;
}

Server::~Server(){}

void Server::SignalHandler(int signum)
{
    (void)signum;
    std::cout<<std::endl<<GRE<<"---------->        SIGNAL RECIEVED        <----------"<<GRE<<std::endl;
    Server::Signal = true;
}

void Server::closeFDS()
{
    for(size_t i = 0 ; i < clients.size(); i++)
    {
        int  xp = clients[i].getFD();
        xp -= 3; // xp add xp just to make it easy for user

        // std::cout<<RED<<"SERVER : CLIENT <"<<clients[i].getFD()<<"> Disconnected"<<WHI<<std::endl;

        std::cout<<RED<<"SERVER : CLIENT <"<<xp<<"> DISCONNECTED AT ";
        Msg_resv();
        std::cout<<"\n"<<WHI;
        close(clients[i].getFD());
    }
    if(fd_soket == -1)
    {
        fd_soket -= 3;
        std::cout<<RED<<"SERVER <"<<fd_soket<<">  DISCONNECTED AT";
        Msg_resv();
        std::cout<<"\n"<<WHI;
        close(fd_soket);
    }
        
}

void Server::Sersocket()
{  
    int option_value = 1;
    struct sockaddr_in add;

    struct pollfd Npoll;
    // struct pollfd newFd; // removed 
   

    add.sin_family = AF_INET;
    add.sin_port = htons(this->port);
    add.sin_addr.s_addr = INADDR_ANY; 
   
    fd_soket = socket(AF_INET, SOCK_STREAM, 0);

    if(fd_soket == -1)
    throw(std::runtime_error("-----> CANT CREAT A VALID SOKET <-----"));
         
    if(setsockopt(fd_soket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value)) == -1)// setsockopt == set socket option
        throw(std::runtime_error("-----> FAILED TO SET OPTION <SO_REUSEADDR> ON SOKET <-----"));
    if(fcntl(fd_soket, F_SETFL, O_NONBLOCK) == -1)
        throw(std::runtime_error("-----> FAILED TO SET OPTION <O_NONBLOCK> ON SOKET <-----"));
   //bind then listen to this server so u can use it onw IP or the same port as befor
   if(bind(fd_soket,(struct sockaddr *)&add, sizeof(add)) == -1)
        throw(std::runtime_error("-----> FAILED TO BIND THE SOKET <-----"));
    if(listen(fd_soket ,SOMAXCONN) == -1)
        throw(std::runtime_error("-----> FAILED TO LISTEN ON SOCKET <-----"));


    Npoll.fd = fd_soket; // chmn fd <socket> bari tminitori 
    Npoll.events = POLLIN; // POLLIN mean that ur ready to read data 
    Npoll.revents = 0;
    fds.push_back(Npoll); //ADD THE INFO FROM THE NPOLL STRUCT TO THE FDS VECTORS
}


void Server::setServerName(std::string serverName)
{
    this->serverName = serverName;
}

std::string Server::getServerName()
{
    return this->serverName;
}


void Server::ServerInit(int port, std::string password)
{
   
    std::cout << YEL << "----->        TYPE YES IF YOU WANT NO GEVE THE SERVER A NAME  OR NO TO USE THE DEFAULT NAME        <-----" << WHI << std::endl;
    while(1)
    {
        std::string name;
        std::cin>>name;
        if(name == "YES" || name == "yes")
        {
            std::cout<< YEL <<"----->        ENTER THE SERVER NAME        <-----"<< WHI<<std::endl;
            std::cin>>name;
            setServerName(name + " ");
            std::cout<< YEL <<"----->        SERVER NAME IS : "<< getServerName() << WHI<<std::endl;
            break;
        }
        else if(name == "NO" || name == "no")
        {
            setServerName("0.Matrix ");
            std::cout<<YEL<<"----->        SERVER DEFAULT NAME IS : "<< getServerName() << WHI<<std::endl;
            break;
        }
        else
        {
            std::cout<<RED<<"----->        INVALID INPUT        <-----"<<WHI<<std::endl;
            continue;
        }
    }
    this->port = port;
    this->password = password;
    Sersocket();

    std::cout<<ORNG<<"----->        SERVER      CONNECTED        <------"<<WHI<<std::endl;
    std::cout<<BLUE<<"          waitng to accept a connection        "<<WHI<<std::endl;

    while(Server::Signal == false)
    {
        if((poll(&fds[0],fds.size(),-1) == -1) && Server::Signal == false) //-> wait for an event
            throw(std::runtime_error("------> ERROR IN POLL <------"));
        for(size_t i = 0; i < fds.size(); i++)
        {
            if(fds[i].revents & POLLIN) // check for data to read
            {
                if(fds[i].fd == fd_soket)
                    AcceptNewclient();
                else
                    ReceiveNewData(fds[i].fd);
            }
        }
    }
    closeFDS();
}




void Server::AcceptNewclient()
{
   Client New_client;

   struct sockaddr_in add_client;
   struct pollfd New_poll;

   socklen_t len_socket = sizeof(add_client);

    int accept_clinet = accept(fd_soket, (sockaddr *)&(add_client), &len_socket);

    if(accept_clinet  == - 1)
        std::cout<<RED<<"----------> ACCEPT FAILED <----------"<<WHI<<std::endl; // used for TCP ---> Transmission Control Protocol
    if(fcntl(accept_clinet, F_SETFL, O_NONBLOCK) == -1)
        throw(std::runtime_error("----->  FAILED TO SET OPTION <O_NONBLOCK> ON SOKET <-----")); // runtime error throw

    New_poll.fd = accept_clinet;
    New_poll.events = POLLIN; // ready to start rading data 
    New_poll.revents = 0;

    New_client.setFD(accept_clinet);
    std::string ip = inet_ntoa(add_client.sin_addr);
    New_client.setIPadd(inet_ntoa(add_client.sin_addr)); // convert the ip to readable string
    
    clients.push_back(New_client); //add the client to the vector of clients
    fds.push_back(New_poll); // add the client socket to the struct pollfd
    
    accept_clinet -= 3;
    std::cout<<GRE<<"------>        CLIENT <"<<accept_clinet<<"> CONNECTED AT";
    Msg_resv();
    std::cout<<GRE<<"        <------\n"<<WHI;

   
}

bool Server::findUserNick(std::string nick)
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(clients[i].getNickName() == nick)
            return true;
    }
    return false;
}

Client Server::getUserbyNick(std::string nick)
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(clients[i].getNickName() == nick)
            return clients[i];
    }
    return Client();
}
Client* Server::getUserbyFD(int fd)
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(clients[i].getFD() == fd)
            return &clients[i];
    }
    return NULL;
}


void Server::findCommand(int fd, std::string command, std::string param)
{
    Client *cl = getUserbyFD(fd);
    if(cl[0].getFD() != fd )
    {
        std::cout<<RED<<"ERROR : CLIENT NOT FOUND"<<WHI<<std::endl;
        return;
    }





    if(command == "")
        return;

    else if(command == "CAP" || command == "LS")
        return;

    else if(command == "PASS")
    {
        std::string pass = param.substr(0, param.find(" "));
        if(pass != this->password)
        {
            std::string msg = ":" + this->serverName   + ERR_PASSWDMISMATCH(cl[0].getNickName());
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        else
        {
            cl[0].setSigned(true);
            std::stringstream ss;
            ss << (cl[0].getFD() - 3);
            // std::string response = "ID "  + ss.str() + ": 001 Welcome to the 0.Matrix\r\n";
            std::string response = RPL_WELCOME(cl[0].getNickName(), ":" + this->serverName );
            send(fd, response.c_str(), response.size(), 0);
        }
        return;
    }

    
    else if(command == "NICK" && cl[0].getSigned())
    {
        std::string nickName = param.substr(0, param.find(" "));
        if(nickName.empty())
        {
            cl[0].setNickName("Guest");
            std::string msg = ":" + this->serverName   + ERR_NEEDMOREPARAMS(cl[0].getNickName(), command);
            send(fd, msg.c_str(), msg.size(), 0);
            cl[0].setNickName("");
            return;
        }
        else
        {
            if(findUserNick(nickName))
            {
                cl[0].setNickName(nickName + "_");
                std::string msgNick = ERR_NICKNAMEINUSE(cl[0].getNickName(), cl[0].getNickName()); /// TODO check
                send(fd , msgNick.c_str(), msgNick.size(), 0);
            }
            else
            {
                cl[0].setNickName(nickName);
                std::string msgNick = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " NICK :" + nickName + "\r\n";
                send(fd , msgNick.c_str(), msgNick.size(), 0);
            }
        }
        return;
    }
    else if (command == "USER" && cl[0].getSigned())
    {
        std::istringstream iss(param);
        std::string userName, realName, ipAdresse, star ,zero;
        iss	>> userName;
        if (userName.empty())
        {
            std::string msg = ":" + this->serverName   + ERR_NEEDMOREPARAMS(cl[0].getNickName(), command);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        cl[0].setUserName(userName);
        iss >> zero;
        iss >> star;
        iss >> realName;
        while (iss >> realName)
            realName += " " + realName;
        cl[0].setRealName(realName);
        // TODO: CHECK THIS
        // iss >> ipAdresse;
        // cl[0].setIPadd(ipAdresse);
        return;
    }
    else if (command == "JOIN" && cl[0].getSigned())
    {
        if (cl[0].getUserName().empty() || cl[0].getNickName().empty() || cl[0].getRealName().empty())
        {
            std::string msg = ":" + this->serverName   + ERR_NOTREGISTERED(cl[0].getNickName());
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        std::string channelName = param.substr(0, param.find(" "));
        std::string key = param.substr(param.find(" ") + 1);
        if (channelName[0] != '#')
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        if (!findChanel(channelName))
        {
            Createchanel(channelName, "", "");
            addClientToChannel(cl[0], channelName);
            Channel channel;
            for(size_t i = 0; i < channels.size(); i++)
            {
                if(channels[i].getChannelName() == channelName)
                {
                    channel = channels[i];
                    break;
                }
            }
            channel.setPermission(cl[0], true);

            std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + ".IP JOIN " + channelName + " * " + cl[0].getRealName() + "\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
        }
        else
        {
            // add user to the channel
            Channel channel = getChannelbyName(channelName);
            
            if(channel.getHasPassword() && channel.getPassword() != key)
            {
                std::string msg = ":" + this->serverName   + ERR_BADCHANNELKEY(cl[0].getNickName(), channelName);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }

            if (channel.getInviteOnly())
            {
                std::string msg = ":" + this->serverName   + ERR_INVITEONLYCHAN(cl[0].getNickName(), channelName);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }

            if (channel.getHasLimit() && channel.getUserCount() >= channel.getLimit())
            {
                std::string msg = ":" + this->serverName   + ERR_CHANNELISFULL(cl[0].getNickName(), channelName);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }

            addClientToChannel(cl[0], channelName);
            channel.setPermission(cl[0], false);
            std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " JOIN :" + channelName + "\r\n";
            send_to_members(channelName, msg, -1);

        }

        Channel channel = getChannelbyName(channelName);
        std::string list = ":0.Matrix 353 " + cl[0].getNickName() + " = " + channelName + " :";
        std::vector<Client> users = channel.getClients();
        for(std::vector<Client>::iterator it = users.begin(); it != users.end(); ++it)
        {
            if (it->getOperator())
                list += "@" + it->getNickName() + " ";
            else
                list += it->getNickName() + " ";
        }
        list += "\r\n";
        send(fd, list.c_str(), list.size(), 0);
        std::string msg = ":" + this->serverName   + RPL_ENDOFNAMES(cl[0].getNickName(), channelName);
        send(fd, msg.c_str(), msg.size(), 0);
        
    }
   
    else if (command == "WHO" || command == "who")
    {
        std::string channelName;
        std::istringstream iss(param);
        iss >> channelName;
        if (channelName[0] != '#')
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }

        Channel channel = getChannelbyName(channelName);
        if (findChanel(channelName))
        {
            std::vector<Client> users = channel.getClients();
            for(std::vector<Client>::iterator it = users.begin(); it != users.end(); ++it)
            {
                Client user = *it;
                std::string msg;
                if (user.getOperator())
                    msg = ":" + this->serverName   + RPL_WHOREPLY(cl[0].getNickName(), channelName, user.getUserName(), user.getIPadd(), "0.Matrix", user.getNickName(), "@x", user.getRealName());
                else
                    msg = ":" + this->serverName   + RPL_WHOREPLY(cl[0].getNickName(), channelName, user.getUserName(), user.getIPadd(), "0.Matrix", user.getNickName(), "x", user.getRealName());
                send(fd, msg.c_str(), msg.size(), 0);
            }
            std::string msg = ":" + this->serverName   + RPL_ENDOFWHO(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
    else if(command == "KICK")
    {
        std::string channelName, nickName, reason;
        std::istringstream iss(param);
        iss >> channelName;
        iss >> nickName;
        iss >> reason;
        if (channelName[0] != '#')
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        if (!findChanel(channelName))
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        Channel channel = getChannelbyName(channelName);
        bool isOp = channel.getPermission(cl[0], "operator");
        if(isOp)
        {
            Client usr = getUserbyNick(nickName);
            if(cl[0].getNickName() == usr.getNickName())
            {
                std::string msg = ":" + this->serverName   + "BAD TRIP AZEBI "; //TODO REMOUVE
                send_to_members(channelName, msg, -1);
            }
            else
            {
                channel.removeClient(usr);
                std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " KICK " + channelName + " " + nickName + " :Kicked by " + cl[0].getNickName() + "\r\n";
                send_to_members(channelName, msg, -1);

            }
        }
        else
        {
            std::string msg = ":" + this->serverName   + ERR_CHANOPRIVSNEEDED(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
        }  
    }
    else if (command == "PRIVMSG")
    {
        std::string sendTo, message;
        std::istringstream iss(param);
        iss >> sendTo;
        std::getline(iss, message);
        if (sendTo.empty())
        {
            std::string msg = ":" + this->serverName   + ERR_NORECIPIENT(cl[0].getNickName(), command);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        if (sendTo[0] == '#')
        {
            if(message.empty())
            {
                std::string msg = ":" + this->serverName   + ERR_NOTEXTTOSEND(cl[0].getNickName());
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            if (!findChanel(sendTo))
            {
                std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), sendTo);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            Channel channel = getChannelbyName(sendTo);
            std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " PRIVMSG " + sendTo + message + "\r\n";
            send_to_members(sendTo, msg , fd);
        }
        else
        {
            bool newClient = findUserNick(sendTo);
            if (!newClient)
            {
                std::string msg = ":" + this->serverName   + ERR_NOSUCHNICK(cl[0].getNickName(), sendTo);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            Client receiver = getUserbyNick(sendTo);
            std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " PRIVMSG " + sendTo + message + "\r\n";
            send(receiver.getFD(), msg.c_str(), msg.size(), 0);
        }
    }
    else if(command == "INVITE")
    {
        std::string nickName, channelName;
        std::istringstream iss(param);
        iss >> nickName;
        iss >> channelName;
        if (channelName[0] != '#')
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        if (!findChanel(channelName))
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        Channel channel = getChannelbyName(channelName);
        bool isOp = channel.getPermission(cl[0], "operator");
        if(isOp)
        {
            Client Invite = getUserbyNick(nickName);
            if (Invite.getNickName().empty())
            {
                std::string msg = ":" + this->serverName   + ERR_NOSUCHNICK(cl[0].getNickName(), nickName);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            channel.addClient(Invite);
            channel.setPermission(cl[0], false);
            std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " INVITE " + nickName + " " + channelName + "\r\n";
            send(Invite.getFD(), msg.c_str(), msg.size(), 0);
        }
        else
        {
            std::string msg = ":" + this->serverName   + ERR_CHANOPRIVSNEEDED(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
        }

    }
    else if (command == "TOPIC")
    {
        std::string channelName, topic;
        std::istringstream iss(param);
        iss >> channelName;
        iss >> topic;
        if (channelName[0] != '#')
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        if (!findChanel(channelName))
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        Channel channel = getChannelbyName(channelName);
        bool isOp = cl[0].getOperator();
        if(isOp)
        {
            channel.setTopic(topic);
            std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " TOPIC " + channelName + " :" + topic + "\r\n";
            send_to_members(channelName, msg, -1);
        }
        else
        {
            std::string msg = ":" + this->serverName   + ERR_CHANOPRIVSNEEDED(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
    else if (command == "QUIT")
    {
        std::string reason;
        std::istringstream iss(param);
        iss >> reason;
        if (reason.empty())
            reason = "Client Quit";
        std::string msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " QUIT :" + reason + "\r\n";
        send_to_members("", msg, -1);
        Clearclients(fd);
    }
    
    else if (command == "MODE")
    {
        std::string channelName, mode, arg, msg;
        std::istringstream iss(param);
        int limit;
        iss >> channelName;
        iss >> mode;
        iss >> arg;
        if (channelName[0] != '#')
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        if (!findChanel(channelName))
        {
            std::string msg = ":" + this->serverName   + ERR_NOSUCHCHANNEL(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        
        Channel channel = getChannelbyName(channelName);
        
        if(mode.empty())
        {
            msg = ":" + this->serverName   + RPL_CHANNELMODEIS(cl[0].getNickName(), channelName, channel.getModes());
            send(fd, msg.c_str(), msg.size(), 0);
            std::string msg = ":" + this->serverName   + RPL_CREATIONTIME(cl[0].getNickName(), channelName, channel.getCreationTime());
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }

        if (!channel.getPermission(cl[0], "operator"))
        {
            std::string msg = ":" + this->serverName   + ERR_CHANOPRIVSNEEDED(cl[0].getNickName(), channelName);
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }

        if(mode == "+i" )
        {
            if (channel.getInviteOnly())
                return;
            channel.setInviteOnly(true);
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " +i\r\n";
            send_to_members(channelName, msg, -1);
            return;
        }
        if(mode == "-i")
        {
            if (!channel.getInviteOnly())
                return;
            channel.setInviteOnly(false);
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " -i\r\n";
            send_to_members(channelName, msg, -1);
        }
        if(mode == "+k")
        {
            if (arg.empty())
            {
                std::string msg = ":" + this->serverName   + ERR_NEEDMOREPARAMS(cl[0].getNickName(), mode);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            channel.setHasPassword(true);
            channel.setPassword(arg);
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " +k " + arg + "\r\n";
            send_to_members(channelName, msg, -1);
        }
        if(mode == "-k")
        {
            channel.setHasPassword(false);
            channel.setPassword("");
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " -k\r\n";
            send_to_members(channelName, msg, -1);
        }

        if(mode == "+t")
        {
            channel.setHasTopic(true);
            channel.setTopic(arg);
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " +t\r\n";
            send_to_members(channelName, msg, -1);
        }


        if(mode == "-t")
        {
            channel.setHasTopic(false);
            channel.setTopic("");
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " -t\r\n";
            send_to_members(channelName, msg, -1);
        }

        if(mode == "+l")
        {
            if (arg.empty())
            {
                std::string msg = ":" + this->serverName   + ERR_NEEDMOREPARAMS(cl[0].getNickName(), mode);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            limit = atoi(arg.c_str());
            if (limit < 0)
                return;
    
            channel.setHasLimit(true);  
            limit = atoi(arg.c_str());
            channel.setLimit(limit);
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " +l " + arg + "\r\n";
            send_to_members(channelName, msg, -1);
        }


        if(mode == "-l")
        {
            channel.setHasLimit(false);
            channel.setLimit(MAX_CLIENTS);
            msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " -l\r\n";
            send_to_members(channelName, msg, -1);
        }

        if(mode == "+o")
        {
            if (arg.empty())
            {
                std::string msg = ":" + this->serverName   + ERR_NEEDMOREPARAMS(cl[0].getNickName(), mode);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            else
            {
                Client op = getUserbyNick(arg);
                for(size_t i = 0; i < clients.size(); i++)
                {
                    if(clients[i].getNickName() == arg)
                    {
                        op = clients[i];
                        break;
                    }
                }
                if (op.getNickName().empty())
                {
                    std::string msg = ":" + this->serverName   + ERR_NOSUCHNICK(cl[0].getNickName(), arg);
                    send(fd, msg.c_str(), msg.size(), 0);
                    return;
                }
                // TODO: IMPLEMENT OPERATOR
                // channel.setPermission(*op, true);
                msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " +o " + arg + "\r\n";
                send_to_members(channelName, msg, -1);
            }
        }
        else if(mode == "-o")
        {
            if (arg.empty())
            {
                std::string msg = ":" + this->serverName   + ERR_NEEDMOREPARAMS(cl[0].getNickName(), mode);
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            else
            {
                Client op = getUserbyNick(arg);
                for(size_t i = 0; i < clients.size(); i++)
                {
                    if(clients[i].getNickName() == arg)
                    {
                        op = clients[i];
                        break;
                    }
                }
                if (op.getNickName().empty())
                {
                    std::string msg = ":" + this->serverName   + ERR_NOSUCHNICK(cl[0].getNickName(), arg);
                    send(fd, msg.c_str(), msg.size(), 0);
                    return;
                }
                //TODO: IMPLEMENT OPERATOR
                // channel.setPermission(*op, false);
                msg = ":" + cl[0].getNickName() + "!" + cl[0].getUserName() + "@" + cl[0].getIPadd() + " MODE " + channelName + " -o " + arg + "\r\n";
                send_to_members(channelName, msg, -1);
            }

        }
    }
    else if(command == "BOT")
    {
        
    }

}

































void Server::send_to_members(std::string channelName, std::string msg, int fd)
{
    Channel chanel;
    for(size_t i = 0; i < channels.size(); i++)
    {
        if(channels[i].getChannelName() == channelName)
        {
            chanel = channels[i];
            break;
        }
    }
    t_ch_access *tmp = chanel.getAccess();
    while (tmp != NULL)
    {
        if (tmp->cl.getFD() != fd)
            send(tmp->cl.getFD(), msg.c_str(), msg.size(), 0);
        tmp = tmp->next;
    }
    return;
}

void Server::addClientToChannel(Client cl, std::string channelName)
{
    for(size_t i = 0; i < channels.size(); i++)
    {
        if(channels[i].getChannelName() == channelName)
        {
            if(channels[i].getUserCount() >= channels[i].getLimit())
            {
                std::string msg = ":" + this->serverName  + ERR_CHANNELISFULL(cl.getNickName(), channelName);
                send(cl.getFD(), msg.c_str(), msg.size(), 0);
                return;
            }
            channels[i].addClient(cl);
            
            break;
        }
    }
    return;
}

bool Server::findChanel(std::string channelName)
{
    for(size_t i = 0; i < channels.size(); i++)
    {
        if(channels[i].getChannelName() == channelName)
            return true;
    }
    return false;
}

void Server::executeCommand(int fd, char *buffer)
{
    char *token = strtok(buffer, "\r\n");
    while (token != NULL)
    {
        std::string command = token;
        std::string command_name = command.substr(0, command.find(" "));
        std::string command_param = command.substr(command.find(" ") + 1);
        findCommand(fd, command_name, command_param);
        token = strtok(NULL, "\r\n");
    }
}



void Server::ReceiveNewData(int fd)
{
    char buffer[1024]; // 1024 bytes 1Kb 
    memset(buffer, 0, sizeof(buffer)); // clear the buffer so i can evoid any leaks/erorrs 

    ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);//The recv method is essential for receiving data in a network socket programming

    int counter = 0;
    if(bytes <= 0)
    {
        counter =fd - 3;
        std::cout << RED<< "------>        CLIENT <" << counter << "> DISCONNECTED AT ";
        Msg_resv();
        std::cout<<"        <------\n"<<WHI;
        Clearclients(fd); // fd client  i added counter here just to khow the number of clients <ID>
        close(fd);
    }
    else
    {
        std::string get_time = getCurrentTime();
        buffer[bytes] = '\0';
        // Msg_resv();
        // std::cout<<GRE<<" CLIENT <"<<fd - 3<<"> :: "<<WHI<<std::endl;
        // std::cout<<buffer<<std::endl;

        executeCommand(fd, buffer);
        fd -=3;

       

        // here to check for commands 

    }
        
}

void Server::Clearclients(int fd)
{
    for(size_t i = 0; i < fds.size(); i++) // remove client from the pollfd struct 
    {
        if(fds[i].fd == fd)
        {
            fds.erase(fds.begin() + i);
            break;
        }
    }
    for(size_t i = 0; i < fds.size();i++) // remove client from the clients vector 
    {
        if(clients[i].getFD() == fd )
        {
            clients.erase(clients.begin() + i);
            break;
        }
    }
}

std::string Server::getCurrentTime()
{
    std::time_t now = std::time(0); // Get current time as a time_t object
    std::tm *ltm = std::localtime(&now); // Convert to local time

    char buffer[80];
    // Format: HH:MM:SS
    std::strftime(buffer, sizeof(buffer), "<%H:%M>", ltm);
    return std::string(buffer);
}

void Server::Msg_resv()
{
    std::string get_time = getCurrentTime();

    std::cout << " [" << get_time << "]";
}

Channel Server::getChannelbyName(std::string channelName)
{
    for(size_t i = 0; i < channels.size(); i++)
    {
        if(channels[i].getChannelName() == channelName)
            return channels[i];
    }
    return Channel();
}
