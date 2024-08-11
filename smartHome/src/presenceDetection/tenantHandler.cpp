#include "tenantHandler.h"
#include <env.h> // Include only if needed here

void TenantHandler::initTenants()
{
    // Initialize tenants with predefined IDs and IPs
    tenants.push_back(Tenant(TENANT1_ID, TENANT1_IP));
    tenants.push_back(Tenant(TENANT2_ID, TENANT2_IP));
    tenants.push_back(Tenant(TENANT3_ID, TENANT3_IP));
}

void TenantHandler::addTenant(const char *name, IPAddress ip)
{
    for (auto &tenant : tenants)
    {
        if (strcmp(tenant.tenantName, name) == 0)
        {
            Serial.print("Overwriting tenant: ");
            Serial.println(name);
            tenant.tenant_ip = ip;
            return;
        }
    }
    Serial.print("Adding tenant: ");
    Serial.println(name);
    tenants.emplace_back(name, ip);
}

void TenantHandler::removeTenant(const char *name)
{
    auto it = std::find_if(tenants.begin(), tenants.end(), [name](const Tenant &c)
                           { return strcmp(c.tenantName, name) == 0; });
    if (it != tenants.end())
    {
        Serial.print("Removing tenant");
        tenants.erase(it);
    }
    else
    {
        Serial.print("Remove failed: tenant not found");
    }
}

bool TenantHandler::isTenantHome(Tenant &tenant)
{
    // Determine if a tenant is home based on ping response
    if (Ping.ping(tenant.tenant_ip) > 0)
    {
        tenant.verifyAttempts = 0;
        if (!tenant.isHome)
        {
            tenantsHome++;
            tenant.isHome = true;
        }
        return true;
    }
    else if (tenant.isHome)
    {
        tenant.verifyAttempts++;
        if (tenant.verifyAttempts >= PING_ATTMEPTS)
        {
            tenant.isHome = false;
            tenant.verifyAttempts = 0;
            tenantsHome--;
            return false;
        }
    }
    return tenant.isHome;
}

bool TenantHandler::pingNextTenant()
{
    if (tenants.empty())
    {
        return false;
    }

    if (currentTenantIndex >= tenants.size())
    {
        currentTenantIndex = 0;
    }
    bool tenantStatus = isTenantHome(tenants[currentTenantIndex]);
    currentTenantIndex++;

    return tenantStatus;
}

bool TenantHandler::allTenantsPinged()
{
    // Check if all tenants have been pinged. resets ptr if all tenants have been pinged
    if (currentTenantIndex >= tenants.size())
    {
        currentTenantIndex = 0;
        return true;
    }
    return false;
}

String TenantHandler::getHomeTenants()
{
    String names;
    for (const auto &tenant : tenants)
    {
        if (tenant.isHome)
        {
            if (!names.isEmpty())
            {
                names += "\n"; // add newline if not the first name
            }
            names += tenant.tenantName;
        }
    }
    return names;
}

int TenantHandler::getHomeTenantsCount() const
{
    return tenantsHome;
}
