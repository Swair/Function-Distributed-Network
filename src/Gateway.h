#ifndef __GATEWAY_H
#define __GATEWAY_H

#include <memory>
#include <stdio.h>
#include <map>
#include "Common.h"


class Gateway
{
    public:
        static Gateway& getInstance();
        ~Gateway(); 
        void route(std::string& response, const std::string& request);
        void reg(const std::string& method, std::function<void(std::string&, const std::string&)> func); 

    private:
        Gateway();
        std::unordered_map<std::string, std::function<void(std::string&, const std::string&)> > routeTable_;

};





#endif



