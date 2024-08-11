#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <WiFi.h>
#include <vector>
#include <ESPping.h>

#define PING_BETWEEN_CLIENTS 5000L  // time between each client ping in milliseconds
#define PING_BEFORE_CLIENTS 400000L // time before first client ping in milliseconds
#define PING_ATTMEPTS 3             // number of attempts to ping a client before considering him not home

struct Client
{
    char *clientName;       // name identifier for the client
    IPAddress client_ip;    // IMPORTANT: the ip address of the client must be static.
    bool isHome;            // true if client's ip is reachable when pinged
    int verifyAttempts = 0; // number of failed attempts to ping client; if it reaches 3, client is considered not home

    Client(const char *name, IPAddress ip, bool home = false) : clientName(const_cast<char *>(name)), client_ip(ip), isHome(home) {}
};

class ClientHandler
{
    std::vector<Client> clients;
    int clientsHome = 0;
    size_t currentClientIndex = 0;

public:
    void initClients();
    void addClient(const char *name, IPAddress ip);
    void removeClient(const char *name);
    bool pingNextClient();
    bool allClientsPinged() const;
    bool isClientHome(Client &client);
    String getHomeClients();
    int getHomeClientsCount() const;
};

#endif // CLIENT_HANDLER_H
