#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
using namespace std;

int test_get_host_by(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <domain>" << endl;
        return -1;
    }
    const char* adomain = argv[1];
    struct hostent* host = gethostbyname(adomain);
    if (host == nullptr) {
        herror("gethostbyname");
        return -1;
    }
    cout << "h_name: " << host->h_name << endl;
    cout << "h_aliases: " << endl;
    for (int i = 0; host->h_aliases[i] != nullptr; ++i) {
        cout << "\t" << host->h_aliases[i] << endl;
    }
    cout << "h_addrtype: " << host->h_addrtype << endl;
    cout << "h_length: " << host->h_length << endl;
    cout << "h_addr_list: " << endl;
    for (int i = 0; host->h_addr_list[i] != nullptr; ++i) {
        // 点分十进制
        cout << "\t" << inet_ntoa(*(struct in_addr*)host->h_addr_list[i]) << endl;
    }
    struct in_addr addr;
    bzero(&addr, sizeof(addr));
    addr.s_addr = inet_addr("127.0.0.1");
    auto domain =  gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (domain == nullptr) {
        herror("gethostbyaddr");
        return -1;
    }
    cout << "h_name: " << domain->h_name << endl;
    cout << "h_aliases: " << endl;
    for (int i = 0; domain->h_aliases[i] != nullptr; ++i) {
        cout << "\t" << domain->h_aliases[i] << endl;
    }
    cout << "h_addrtype: " << domain->h_addrtype << endl;
    cout << "h_length: " << domain->h_length << endl;
    cout << "h_addr_list: " << endl;
    for (int i = 0; domain->h_addr_list[i] != nullptr; ++i) {
        // 点分十进制
        cout << "\t" << inet_ntoa(*(struct in_addr*)domain->h_addr_list[i]) << endl;
    }
    return 0;
}


int test_get_service_by_name(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <service>" << endl;
        return -1;
    }
    const char* service = argv[1];
    
    if (argc == 3) {

    }
    struct servent* serv = getservbyname(service, nullptr);
    if (serv == nullptr) {
        herror("getservbyname");
        return -1;
    }
    cout << "s_name: " << serv->s_name << endl;
    cout << "s_aliases: " << endl;
    for (int i = 0; serv->s_aliases[i] != nullptr; ++i) {
        cout << "\t" << serv->s_aliases[i] << endl;
    }
    cout << "s_port: " << ntohs(serv->s_port) << endl;
    cout << "s_proto: " << serv->s_proto << endl;
    return 0;
}

int main(int argc , char* argv[]) {
    //test_get_host_by(argc, argv);
    test_get_service_by_name(argc, argv);
}
