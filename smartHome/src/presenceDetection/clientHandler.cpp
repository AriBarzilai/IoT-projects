#include "clientHandler.h"
#include <env.h> // Include only if needed here

void ClientHandler::initClients()
{
    // Initialize clients with predefined IDs and IPs
    clients.push_back(Client(CLIENT1_ID, CLIENT1_IP));
    clients.push_back(Client(CLIENT2_ID, CLIENT2_IP));
    clients.push_back(Client(CLIENT3_ID, CLIENT3_IP));
}

void ClientHandler::addClient(const char *name, IPAddress ip)
{
    for (auto &client : clients)
    {
        if (strcmp(client.clientName, name) == 0)
        {
            Serial.print("Overwriting client: ");
            Serial.println(name);
            client.client_ip = ip;
            return;
        }
    }
    Serial.print("Adding client: ");
    Serial.println(name);
    clients.emplace_back(name, ip);
}

void ClientHandler::removeClient(const char *name)
{
    auto it = std::find_if(clients.begin(), clients.end(), [name](const Client &c)
                           { return strcmp(c.clientName, name) == 0; });
    if (it != clients.end())
    {
        Serial.print("Removing client");
        clients.erase(it);
    }
    else
    {
        Serial.print("Remove failed: client not found");
    }
}

bool ClientHandler::isClientHome(Client &client)
{
    // Determine if a client is home based on ping response
    if (Ping.ping(client.client_ip) > 0)
    {
        client.verifyAttempts = 0;
        if (!client.isHome)
        {
            clientsHome++;
            client.isHome = true;
        }
        return true;
    }
    else if (client.isHome)
    {
        client.verifyAttempts++;
        if (client.verifyAttempts >= PING_ATTMEPTS)
        {
            client.isHome = false;
            client.verifyAttempts = 0;
            clientsHome--;
            return false;
        }
    }
    return client.isHome;
}

bool ClientHandler::pingNextClient()
{
    if (clients.empty())
    {
        return false;
    }

    if (currentClientIndex >= clients.size())
    {
        currentClientIndex = 0;
    }
    bool clientStatus = isClientHome(clients[currentClientIndex]);
    currentClientIndex++;

    return clientStatus;
}

bool ClientHandler::allClientsPinged() const
{
    // Check if all clients have been pinged
    return currentClientIndex >= clients.size();
}

String ClientHandler::getHomeClients()
{
    String names;
    for (const auto &client : clients)
    {
        if (client.isHome)
        {
            if (!names.isEmpty())
            {
                names += "\n"; // add newline if not the first name
            }
            names += client.clientName;
        }
    }
    return names;
}

int ClientHandler::getHomeClientsCount() const
{
    return clientsHome;
}
