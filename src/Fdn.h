#ifndef __SERVERNODE_H
#define __SERVERNODE_H
#include "Common.h"
#include "Gateway.h"



struct Fdu
{
    std::vector<std::string> addr;
    std::vector<int> validTicks;
    size_t curIx;
};

class Fdn
{
    public:
        static Fdn& getInstance();
        ~Fdn();
        void run(std::string& response, const std::string& request);

    private:
        Fdn();
        json getBody(const std::string& request);
        void getHandle(std::string& response, const std::string& request);
        void rgetHandle(std::string& response, const std::string& request);
        void regHandle(std::string& response, const std::string& request);
        void infoHandle(std::string& response, const std::string& request);
        void monitor();

    private: 
        std::unordered_map<std::string, std::unique_ptr<Fdu> > table_;
        std::mutex mx_;
};



#endif


