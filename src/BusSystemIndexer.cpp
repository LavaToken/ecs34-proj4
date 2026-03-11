#include "BusSystemIndexer.h"
#include <vector>
#include <unordered_map>
#include <algorithm>

struct CBusSystemIndexer::SImplementation
{
    std::shared_ptr<CBusSystem> DBusSystem;
    std::vector<std::shared_ptr<SStop>> DSortedStopsByIndex;
    std::vector<std::shared_ptr<SRoute>> DSortedRoutesByIndex;
    std::unordered_map<TNodeID, std::shared_ptr<SStop>> DStopsByNodeID;
    std::unordered_map<TNodeID, std::unordered_set<std::shared_ptr<SRoute>>> DRoutesByNodeID;

    SImplementation(std::shared_ptr<CBusSystem> bussystem)
    {
        DBusSystem = bussystem;
        for (size_t Index = 0; Index < DBusSystem->StopCount(); Index++)
        {
            auto Stop = DBusSystem->StopByIndex(Index);
            DSortedStopsByIndex.push_back(Stop);
            DStopsByNodeID[Stop->NodeID()] = Stop;
        }
        std::sort(DSortedStopsByIndex.begin(), DSortedStopsByIndex.end(), [](std::shared_ptr<SStop> l, std::shared_ptr<SStop> r) -> bool
                  { return l->ID() < r->ID(); });

        for (size_t Index = 0; Index < DBusSystem->RouteCount(); Index++)
        {
            auto Route = DBusSystem->RouteByIndex(Index);
            DSortedRoutesByIndex.push_back(Route);
            for (size_t StopIndex = 0; StopIndex < Route->StopCount(); StopIndex++)
            {
                auto StopID = Route->GetStopID(StopIndex);
                auto Stop = DBusSystem->StopByID(StopID);
                if (Stop)
                {
                    DRoutesByNodeID[Stop->NodeID()].insert(Route);
                }
            }
        }

        std::sort(DSortedRoutesByIndex.begin(), DSortedRoutesByIndex.end(), [](std::shared_ptr<SRoute> l, std::shared_ptr<SRoute> r) -> bool
                  { return l->Name() < r->Name(); });
    }

    ~SImplementation()
    {
    }

    std::size_t StopCount() const noexcept
    {
        return DBusSystem->StopCount();
    }

    std::size_t RouteCount() const noexcept
    {
        return DBusSystem->RouteCount();
    }

    std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept
    {
        if (index < DSortedStopsByIndex.size())
        {
            return DSortedStopsByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept
    {
        if (index < DSortedRoutesByIndex.size())
        {
            return DSortedRoutesByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept
    {
        auto it = DStopsByNodeID.find(id);
        if (it != DStopsByNodeID.end())
        {
            return it->second;
        }
        return nullptr;
    }

    bool RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>> &routes) const noexcept
    {
        bool found = false;
        auto srcIt = DRoutesByNodeID.find(src);
        auto destIt = DRoutesByNodeID.find(dest);
        if (srcIt != DRoutesByNodeID.end() && destIt != DRoutesByNodeID.end())
        {
            for (auto route : srcIt->second)
            {
                if (destIt->second.find(route) != destIt->second.end())
                {
                    for (size_t i = 0; i < route->StopCount() - 1; i++)
                    {
                        auto stopID1 = route->GetStopID(i);
                        auto stopID2 = route->GetStopID(i + 1);
                        auto stop1 = DBusSystem->StopByID(stopID1);
                        auto stop2 = DBusSystem->StopByID(stopID2);
                        if (stop1 && stop2 && stop1->NodeID() == src && stop2->NodeID() == dest)
                        {
                            routes.insert(route);
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
        return found;
    }

    bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept
    {
        auto srcIt = DRoutesByNodeID.find(src);
        auto destIt = DRoutesByNodeID.find(dest);
        if (srcIt != DRoutesByNodeID.end() && destIt != DRoutesByNodeID.end())
        {
            for (auto route : srcIt->second)
            {
                if (destIt->second.find(route) != destIt->second.end())
                {
                    for (size_t i = 0; i < route->StopCount() - 1; i++)
                    {
                        auto stopID1 = route->GetStopID(i);
                        auto stopID2 = route->GetStopID(i + 1);
                        auto stop1 = DBusSystem->StopByID(stopID1);
                        auto stop2 = DBusSystem->StopByID(stopID2);
                        if (stop1 && stop2 && stop1->NodeID() == src && stop2->NodeID() == dest)
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
};

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem)
{
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

CBusSystemIndexer::~CBusSystemIndexer()
{
}

std::size_t CBusSystemIndexer::StopCount() const noexcept
{
    return DImplementation->StopCount();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept
{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept
{
    return DImplementation->SortedStopByIndex(index);
}

std::shared_ptr<CBusSystemIndexer::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept
{
    return DImplementation->SortedRouteByIndex(index);
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept
{
    return DImplementation->StopByNodeID(id);
}

bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>> &routes) const noexcept
{
    return DImplementation->RoutesByNodeIDs(src, dest, routes);
}

bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept
{
    return DImplementation->RouteBetweenNodeIDs(src, dest);
}
