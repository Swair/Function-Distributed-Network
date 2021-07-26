#include "Gateway.h"

Gateway& Gateway::getInstance()
{
    static Gateway gw;
    return gw;
}

Gateway::Gateway()
{}

Gateway::~Gateway()
{}

void Gateway::reg(const std::string& method, std::function<void(std::string&, const std::string&)> func) 
{
    routeTable_[method] = func;
}

void Gateway::route(std::string& response, const std::string& request)
{
    std::string failCode;
    //log_write("%s\n", request.c_str());
    std::string uri;
    if(0 == getContent(uri, request, "POST", 1, " HTTP"))
    {
        failCode = R"({"code":400, "msg":"fail in get uri"})";
        throw ExceptionPanic(failCode);
    }

    int ix = uri.rfind("/");
    if(ix > 0)
    {
        std::string method = uri.substr(ix + 1);
        //log_write("Gateway::route, %s\n", method.c_str());
        try
        {
            auto f = routeTable_[method];
            f(response, request);
        }
        catch(nlohmann::detail::exception& e)
        {
            std::string msg = e.what();
            failCode = R"({"code":430, "msg":")" + msg + R"("})";
            throw ExceptionPanic(failCode);
        }
        catch(std::exception& e)
        {
            failCode = R"({"code":431, "msg":"fail in route, check the web api"})";
            throw ExceptionPanic(failCode);
        }
        catch(ExceptionPanic& e)
        {
            std::string msg = e.what();
            failCode = R"({"code":432, "msg":")" + msg + R"("})";
            throw ExceptionPanic(failCode);
        }
        catch(...)
        {
            failCode = R"({"code":433, "msg":"fail in route"})";
            throw ExceptionPanic(failCode);
        }
    }
}

