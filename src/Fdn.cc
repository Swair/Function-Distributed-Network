#include "Common.h"
#include "Gateway.h"
#include "TimerTask.h"
#include "Fdn.h"


Fdn& Fdn::getInstance()
{
    static Fdn sn;
    return sn;
}

Fdn::Fdn()
{
    Gateway::getInstance().reg("reg",   std::bind(&Fdn::regHandle, this, _1, _2));
    Gateway::getInstance().reg("get",   std::bind(&Fdn::getHandle, this, _1, _2));
    Gateway::getInstance().reg("rand",  std::bind(&Fdn::rgetHandle, this, _1, _2));
    Gateway::getInstance().reg("info",  std::bind(&Fdn::infoHandle, this, _1, _2));
    TimerMan::getInstance().setWithTi(std::bind(&Fdn::monitor, this), 1);
}

Fdn::~Fdn()
{
}

void Fdn::monitor()
{
    std::unique_lock<std::mutex> l(mx_);
    size_t it = 0;
    for(auto&& item = table_.begin(); item != table_.end(); ++item)
    {
        for(size_t i = 0; i < item->second->validTicks.size(); i++)
        {
            if(0 == item->second->validTicks[i])
            {
                item->second->validTicks.erase(item->second->validTicks.begin() + i);
                item->second->addr.erase(item->second->addr.begin() + i);
            }
            else if(-1 == item->second->validTicks[i])
                continue;
            else
                item->second->validTicks[i] --;
        }
        it ++; 
    }
}

void Fdn::run(std::string& response, const std::string& request)
{
    try
    {
        Gateway::getInstance().route(response, request);
    }
    catch(ExceptionPanic& e)
    {
        response = e.get();
    }
    if(response.empty())
        response = R"({"code":510})";
}


json Fdn::getBody(const std::string& request)
{
    json jRequest;
    int index = request.find("{");
    if( -1 == index)
        throw ExceptionPanic(setRes(401, "algError, the body is not json format"));
    std::string body = request.substr(index, request.size() - index);
    try
    { 
        jRequest = json::parse(body);
    }
    catch(nlohmann::detail::exception& e)
    {   
        std::string msg = e.what();
        throw ExceptionPanic(setRes(401, "algError, request json::parse, " + msg));
    } 
    return jRequest;
}

void Fdn::regHandle(std::string& response, const std::string& request)
{
    std::unique_lock<std::mutex> l(mx_);
    json jResponse;
    json jRequest = getBody(request);
    std::string function = jRequest.at("func");
    std::string address = jRequest.at("addr");
    int validTick = jRequest.at("validTick");
    //log_write("reg function name [%s]\n", function.c_str());

    auto&& item = table_.find(function);
    if(item == table_.end())
        table_[function] = make_unique<Fdu>();

    size_t i = 0;
    for(i = 0; i < table_[function]->addr.size(); i++)
    {
        if(table_[function]->addr[i] == address)
        { 
            table_[function]->validTicks[i] = validTick;
            break;
        }
    }
    if(i == table_[function]->addr.size())
    {
        table_[function]->addr.emplace_back(address);
        table_[function]->validTicks.emplace_back(validTick);
    }
    jResponse["code"] = 200;
    jResponse["id"] = jRequest.at("id");
    response = jResponse.dump();
}

void Fdn::getHandle(std::string& response, const std::string& request)
{
    std::unique_lock<std::mutex> l(mx_);
    json jResponse;
    json jRequest = getBody(request);
    std::string function = jRequest.at("func");
    auto&& item = table_.find(function);
    if(item == table_.end() || item->second->addr.empty())
    {
        jResponse["code"] = 404;
        jResponse["msg"] = "no function found";
    }
    else
    {
        std::string address = item->second->addr[item->second->curIx];
        item->second->curIx = (item->second->curIx + 1) % item->second->addr.size();
        jResponse["code"] = 200;
        jResponse["addr"] = address;
    }
    jResponse["id"] = jRequest.at("id");
    response = jResponse.dump();
}

void Fdn::rgetHandle(std::string& response, const std::string& request)
{
    std::unique_lock<std::mutex> l(mx_);
    json jResponse;
    json jRequest = getBody(request);
    std::string function = jRequest.at("func");

    auto&& item = table_.find(function);
    if(item == table_.end())
    {
        jResponse["code"] = 404;
        jResponse["msg"] = "no function found";
    }
    else
    {
        srand(time(0));
        int randnum = rand() % item->second->addr.size();
        std::string address = item->second->addr[randnum];
        jResponse["code"] = 200;
        jResponse["addr"] = address;
    }
    jResponse["id"] = jRequest.at("id");
    response = jResponse.dump();
}

void Fdn::infoHandle(std::string& response, const std::string& request)
{
    std::unique_lock<std::mutex> l(mx_);
    json jResponse;
    json jRequest = getBody(request);

    size_t it = 0;
    for(auto&& item : table_)
    {
        jResponse["Apis"][it]["func"] = item.first;
        for(size_t i = 0; i < item.second->addr.size(); i++)
            jResponse["Apis"][it]["addr"][i] = item.second->addr[i];
        for(size_t i = 0; i < item.second->validTicks.size(); i++)
            jResponse["Apis"][it]["validTick"][i] = item.second->validTicks[i];
        it ++; 
    }
    jResponse["code"] = 200;
    jResponse["id"] = jRequest.at("id");
    if(0 == it)
        jResponse["msg"] = "no registered function";

    response = jResponse.dump();
}








