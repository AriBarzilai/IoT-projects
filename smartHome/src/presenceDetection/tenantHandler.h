#ifndef TENANT_HANDLER_H
#define TENANT_HANDLER_H

#include <WiFi.h>
#include <vector>
#include <ESPping.h>

#define PING_BETWEEN_TENANTS 5000L  // time between each tenant ping in milliseconds
#define PING_BEFORE_TENANTS 400000L // time before first tenant ping in milliseconds
#define PING_ATTMEPTS 3             // number of attempts to ping a tenant before considering him not home

struct Tenant
{
    char *tenantName;       // name identifier for the tenant
    IPAddress tenant_ip;    // IMPORTANT: the ip address of the tenant must be static.
    bool isHome;            // true if tenant's ip is reachable when pinged
    int verifyAttempts = 0; // number of failed attempts to ping tenant; if it reaches 3, tenant is considered not home

    Tenant(const char *name, IPAddress ip, bool home = false) : tenantName(const_cast<char *>(name)), tenant_ip(ip), isHome(home) {}
};

class TenantHandler
{
    std::vector<Tenant> tenants;
    int tenantsHome = 0;
    size_t currentTenantIndex = 0;

public:
    void initTenants();
    void addTenant(const char *name, IPAddress ip);
    void removeTenant(const char *name);
    bool pingNextTenant();
    bool allTenantsPinged();
    bool isTenantHome(Tenant &tenant);
    String getHomeTenants();
    int getHomeTenantsCount() const;
};

#endif // TENANT_HANDLER_H
