#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"
#include "workflow/WFServer.h"
#include "workflow/WFHttpServer.h"
#include "workflow/WFFacilities.h"
#include "Common.h"
#include "Fdn.h"

unsigned short g_port = 10000;

static WFFacilities::WaitGroup wait_group(1);
class FdnServer
{
    public:

        void runServerNode()
        {
            signal(SIGINT, sig_handler);
            log_write("|---------------------------------------------|\n");
            log_write("|      Funtion Distributed Network Server     |\n");
            log_write("|---------------------------------------------|\n");
            startRunServer();
        }

        void startRunServer()
        {
            WFHttpServer server([=](WFHttpTask* contex){
                    protocol::HttpRequest *req = contex->get_req();
                    protocol::HttpResponse *resp = contex->get_resp();
                    clientInfo(contex);
                    // header
                    const http_parser_t* parser = req->get_parser();
                    std::string request;
                    std::string response;
                    request.resize(parser->msgsize);
                    memcpy((char*)&request[0], parser->msgbuf, parser->msgsize);
                    Fdn::getInstance().run(response, request);
                    resp->append_output_body(response);        
                    });
            if (server.start(g_port) == 0)
            {
                wait_group.wait();
                server.stop();
            }
            else
            {
                perror("Cannot start server");
                exit(1);
            }
        }

        static void sig_handler(int signo)
        {
            wait_group.done();
        }

        void clientInfo(const WFHttpTask* contex)
        {
            long long seq = contex->get_task_seq();

            // print client ip:port
            char addrstr[128];
            struct sockaddr_storage addr;
            socklen_t l = sizeof addr;
            unsigned short port = 0;

            contex->get_peer_addr((struct sockaddr *)&addr, &l);
            if (addr.ss_family == AF_INET)
            {
                struct sockaddr_in *sin = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &sin->sin_addr, addrstr, 128);
                port = ntohs(sin->sin_port);
            }
            else if (addr.ss_family == AF_INET6)
            {
                struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &sin6->sin6_addr, addrstr, 128);
                port = ntohs(sin6->sin6_port);
            }
            else
                strcpy(addrstr, "Unknown");

            fprintf(stderr, "Peer address: %s:%d, seq: %lld.\n",
                    addrstr, port, seq);

        }
};

int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        g_port = atoi(argv[1]);
    }
    FdnServer server; 
    server.runServerNode();

    return 0;
}




