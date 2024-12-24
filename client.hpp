#ifndef CLIENT_HPP
#define CLIENT_HPP



class Client
{
    private:

        int FD;
        std::string addIP;

        std::string UserName;
        std::string NickName;
        std::string RealName;
        std::string Pass;
        bool isSigned;


        bool isOperator;
    
        clock_t start;
        clock_t end;




    public :
        Client();
        ~Client();

        void setFD(int fd);
        int getFD();

        void setIPadd(std::string ipadd);
        std::string getIPadd();

        void setUserName(std::string username);
        std::string getUserName();

        void setNickName(std::string nickname);
        std::string getNickName();

        void setRealName(std::string realname);
        std::string getRealName();

        void setPass(std::string pass);
        std::string getPass();

        void setSigned(bool sign);
        bool getSigned();

        void setOperator(bool op);
        bool getOperator();
        
        void setStart();
        clock_t getStart();

        void setEnd(clock_t end);
        clock_t getEnd();

        Client &operator=(const Client &cl);





};

#endif // CLIENT_HPP
