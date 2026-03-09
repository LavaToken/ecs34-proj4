#include "CSVBusSystem.h"
#include <unordered_map>

struct CCSVBusSystem::SImplementation
{

    struct SStop : public CBusSystem::SStop
    {
        TStopID DID;
        CStreetMap::TNodeID DNodeID;

        ~SStop() override
        {
        }

        TStopID ID() const noexcept override
        {
            return DID;
        }

        CStreetMap::TNodeID NodeID() const noexcept override
        {
            return DNodeID;
        }
    };

    struct SRoute : public CBusSystem::SRoute
    {
        std::string DName;
        std::vector<TStopID> DStopIDs;

        ~SRoute() override
        {
        }

        std::string Name() const noexcept override
        {
            return DName;
        }

        std::size_t StopCount() const noexcept override
        {
            return DStopIDs.size();
        }

        TStopID GetStopID(std::size_t index) const noexcept override
        {
            if (index >= DStopIDs.size())
            {
                return InvalidStopID;
            }
            return DStopIDs[index];
        }
    };

    const std::string STOP_ID_HEADER = "stop_id";
    const std::string NODE_ID_HEADER = "node_id";
    const std::string ROUTE_NAME_HEADER = "route";

    std::vector<std::shared_ptr<SStop>> DStopsByIndex;
    std::unordered_map<TStopID, std::shared_ptr<SStop>> DStopsByID;
    std::vector<std::shared_ptr<SRoute>> DRoutesByIndex;
    std::unordered_map<std::string, std::shared_ptr<SRoute>> DRoutesByName;

    bool ReadStops(std::shared_ptr<CDSVReader> stopsrc)
    {
        std::vector<std::string> TempRow;

        if (stopsrc->ReadRow(TempRow))
        {
            size_t StopColumn = -1;
            size_t NodeColumn = -1;
            for (size_t Index = 0; Index < TempRow.size(); Index++)
            {
                if (TempRow[Index] == STOP_ID_HEADER)
                {
                    StopColumn = Index;
                }
                else if (TempRow[Index] == NODE_ID_HEADER)
                {
                    NodeColumn = Index;
                }
            }
            if (StopColumn == -1 || NodeColumn == -1)
            {
                return false;
            }
            while (stopsrc->ReadRow(TempRow))
            {
                if (TempRow.size() <= std::max(StopColumn, NodeColumn))
                {
                    continue;
                }
                try
                {
                    TStopID StopID = std::stoull(TempRow[StopColumn]);
                    CStreetMap::TNodeID NodeID = std::stoull(TempRow[NodeColumn]);
                    auto NewStop = std::make_shared<SStop>();
                    NewStop->DID = StopID;
                    NewStop->DNodeID = NodeID;
                    DStopsByIndex.push_back(NewStop);
                    DStopsByID[StopID] = NewStop;
                }
                catch (const std::exception &E)
                {
                    continue;
                }
            }

            return true;
        }
        return false;
    }

    bool ReadRoutes(std::shared_ptr<CDSVReader> routesrc)
    {
        std::vector<std::string> TempRow;

        if (routesrc->ReadRow(TempRow))
        {
            size_t RouteColumn = -1;
            size_t StopColumn = -1;
            for (size_t Index = 0; Index < TempRow.size(); Index++)
            {
                if (TempRow[Index] == ROUTE_NAME_HEADER)
                {
                    RouteColumn = Index;
                }
                else if (TempRow[Index] == STOP_ID_HEADER)
                {
                    StopColumn = Index;
                }
            }
            if (RouteColumn == -1 || StopColumn == -1)
            {
                return false;
            }
            while (routesrc->ReadRow(TempRow))
            {
                if (TempRow.size() <= std::max(RouteColumn, StopColumn))
                {
                    continue;
                }
                try
                {
                    std::string RouteName = TempRow[RouteColumn];
                    TStopID StopID = std::stoull(TempRow[StopColumn]);

                    if (DStopsByID.find(StopID) == DStopsByID.end())
                    {
                        continue;
                    }

                    auto It = DRoutesByName.find(RouteName);
                    if (It == DRoutesByName.end())
                    {
                        auto NewRoute = std::make_shared<SRoute>();
                        NewRoute->DName = RouteName;
                        NewRoute->DStopIDs.push_back(StopID);
                        DRoutesByIndex.push_back(NewRoute);
                        DRoutesByName[RouteName] = NewRoute;
                    }
                    else
                    {
                        It->second->DStopIDs.push_back(StopID);
                    }
                }
                catch (const std::exception &E)
                {
                    continue;
                }
            }

            return true;
        }
        return false;
    }

    SImplementation(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc)
    {
        if (ReadStops(stopsrc))
        {
            ReadRoutes(routesrc);
        }
    }

    std::size_t StopCount() const noexcept
    {
        return DStopsByIndex.size();
    }

    std::size_t RouteCount() const noexcept
    {
        return DRoutesByIndex.size();
    }

    std::shared_ptr<CBusSystem::SStop> StopByIndex(std::size_t index) const noexcept
    {
        if (index >= DStopsByIndex.size())
        {
            return nullptr;
        }
        return DStopsByIndex[index];
    }

    std::shared_ptr<CBusSystem::SStop> StopByID(TStopID id) const noexcept
    {
        auto It = DStopsByID.find(id);
        if (It == DStopsByID.end())
        {
            return nullptr;
        }
        return It->second;
    }

    std::shared_ptr<CBusSystem::SRoute> RouteByIndex(std::size_t index) const noexcept
    {
        if (index >= DRoutesByIndex.size())
        {
            return nullptr;
        }
        return DRoutesByIndex[index];
    }

    std::shared_ptr<CBusSystem::SRoute> RouteByName(const std::string &name) const noexcept
    {
        auto It = DRoutesByName.find(name);
        if (It == DRoutesByName.end())
        {
            return nullptr;
        }
        return It->second;
    }
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc)
{
    DImplementation = std::make_unique<SImplementation>(stopsrc, routesrc);
}

CCSVBusSystem::~CCSVBusSystem()
{
}

std::size_t CCSVBusSystem::StopCount() const noexcept
{
    return DImplementation->StopCount();
}

std::size_t CCSVBusSystem::RouteCount() const noexcept
{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept
{
    return DImplementation->StopByIndex(index);
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept
{
    return DImplementation->StopByID(id);
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept
{
    return DImplementation->RouteByIndex(index);
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept
{
    return DImplementation->RouteByName(name);
}
