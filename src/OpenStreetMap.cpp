#include "OpenStreetMap.h"
#include <unordered_map>

struct COpenStreetMap::SImplementation{
    const std::string DOSMTag = "osm";
    const std::string DNodeTag = "node";
    const std::string DWayTag = "way";
    const std::string DTagTag = "tag";
    const std::string DNodeIDAttr = "id";
    const std::string DNodeLatAttr = "lat";
    const std::string DNodeLonAttr = "lon";
    const std::string DWayIDAttr = "id";
    const std::string DTagKeyAttr = "k";
    const std::string DTagValueAttr = "v";
    const std::string DWayNodeTag = "nd";
    const std::string DWayNodeRefAttr = "ref";

    struct SNode: public CStreetMap::SNode{
        TNodeID DID;
        TLocation DLocation;
        std::vector<std::string> DAttributeKeys;
        std::unordered_map<std::string, std::string> DAttributes;

        ~SNode() override{};
        TNodeID ID() const noexcept override{
            return DID;
        }

        TLocation Location() const noexcept override{
            return DLocation;
        }

        std::size_t AttributeCount() const noexcept override{
            return DAttributeKeys.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if(index >= DAttributeKeys.size()){
                return "";
            }
            return DAttributeKeys[index];
        }

        bool HasAttribute(const std::string &key) const noexcept override{
            return DAttributes.find(key) != DAttributes.end();
        }

        std::string GetAttribute(const std::string &key) const noexcept override{
            auto It = DAttributes.find(key);
            if(It == DAttributes.end()){
                return "";
            }
            return It->second;
        }
    };

    struct SWay: public CStreetMap::SWay{
        TWayID DID;
        std::vector<TNodeID> DNodeIDs;
        std::vector<std::string> DAttributeKeys;
        std::unordered_map<std::string, std::string> DAttributes;

        ~SWay() override{};
        TWayID ID() const noexcept override{
            return DID;
        }

        std::size_t NodeCount() const noexcept override{
            return DNodeIDs.size();
        }

        TNodeID GetNodeID(std::size_t index) const noexcept override{
            if(index >= DNodeIDs.size()){
                return InvalidNodeID;
            }
            return DNodeIDs[index];
        }

        std::size_t AttributeCount() const noexcept override{
            return DAttributeKeys.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if(index >= DAttributeKeys.size()){
                return "";
            }
            return DAttributeKeys[index];
        }

        bool HasAttribute(const std::string &key) const noexcept override{
            return DAttributes.find(key) != DAttributes.end();
        }

        std::string GetAttribute(const std::string &key) const noexcept override{
            auto It = DAttributes.find(key);
            if(It == DAttributes.end()){
                return "";
            }
            return It->second;
        }
    };
    std::vector<std::shared_ptr<SNode>> DNodesByIndex;
    std::unordered_map<TNodeID,std::shared_ptr<SNode>> DNodesByID;
    std::vector<std::shared_ptr<SWay>> DWaysByIndex;
    std::unordered_map<TWayID,std::shared_ptr<SWay>> DWaysByID;

    void ParseTags(std::shared_ptr<CXMLReader> src, std::vector<std::string> &keys, std::unordered_map<std::string, std::string> &attrs, const std::string &parentTag){
        SXMLEntity TempEntity;
        while(src->ReadEntity(TempEntity)){ // read tags
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DTagTag){ // tag start
                std::string k = TempEntity.AttributeValue(DTagKeyAttr); // get the tag key
                std::string v = TempEntity.AttributeValue(DTagValueAttr); // get the tag value
                keys.push_back(k);
                attrs[k] = v; // add the tag to the map
            }
            else if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == parentTag){ // tag end
                break; // break out of the loop
            }
        }
    }

    bool ParseNodes(std::shared_ptr<CXMLReader> src, SXMLEntity &nextentity){
        if(nextentity.DType == SXMLEntity::EType::StartElement && nextentity.DNameData == DNodeTag){ // are we starting a node?
            do {
                if(nextentity.DType == SXMLEntity::EType::StartElement && nextentity.DNameData == DNodeTag){ // case 1 are we starting a node?
                    auto NodeID = std::stoull(nextentity.AttributeValue(DNodeIDAttr)); // get the node id
                    auto NodeLat = std::stod(nextentity.AttributeValue(DNodeLatAttr));
                    auto NodeLon = std::stod(nextentity.AttributeValue(DNodeLonAttr));
                    auto NewNode = std::make_shared<SNode>();
                    NewNode->DID = NodeID;
                    NewNode->DLocation = std::make_pair(NodeLat,NodeLon);
                    
                    // Possible attributes (tags)
                    SXMLEntity TagEntity;
                    while(src->ReadEntity(TagEntity)){ // read tags
                        if(TagEntity.DType == SXMLEntity::EType::StartElement && TagEntity.DNameData == DTagTag){ // tag start
                            std::string k = TagEntity.AttributeValue(DTagKeyAttr); // get the tag key
                            std::string v = TagEntity.AttributeValue(DTagValueAttr); // get the tag value
                            NewNode->DAttributeKeys.push_back(k);
                            NewNode->DAttributes[k] = v; // add the tag to the node
                        }
                        else if(TagEntity.DType == SXMLEntity::EType::EndElement && TagEntity.DNameData == DNodeTag){ // tag end
                            break; // break out of the loop
                        }
                    }

                    DNodesByIndex.push_back(NewNode);
                    DNodesByID[NodeID] = NewNode;
                }
                else if(nextentity.DType == SXMLEntity::EType::StartElement && nextentity.DNameData == DWayTag){ // case 2 are we starting a way?
                    return true; // Start of ways
                }
            } while(src->ReadEntity(nextentity)); // read next entity
        }
        return true;
    }


    bool ParseWays(std::shared_ptr<CXMLReader> src, SXMLEntity &nextentity){
        while(nextentity.DType == SXMLEntity::EType::StartElement && nextentity.DNameData == DWayTag){
            auto WayID = std::stoull(nextentity.AttributeValue(DWayIDAttr));
            auto NewWay = std::make_shared<SWay>();
            NewWay->DID = WayID;

            while(src->ReadEntity(nextentity)){
                if(nextentity.DType == SXMLEntity::EType::StartElement && nextentity.DNameData == DWayNodeTag){ // node ref
                    NewWay->DNodeIDs.push_back(std::stoull(nextentity.AttributeValue(DWayNodeRefAttr)));
                }
                else if(nextentity.DType == SXMLEntity::EType::StartElement && nextentity.DNameData == DTagTag){ // tag
                    std::string k = nextentity.AttributeValue(DTagKeyAttr);
                    std::string v = nextentity.AttributeValue(DTagValueAttr);
                    NewWay->DAttributeKeys.push_back(k);
                    NewWay->DAttributes[k] = v;
                }
                else if(nextentity.DType == SXMLEntity::EType::EndElement && nextentity.DNameData == DWayTag){ // way end
                    break;
                }
            }
            DWaysByIndex.push_back(NewWay); // add way to index
            DWaysByID[WayID] = NewWay; // add way to id map
            if(!src->ReadEntity(nextentity)){ // read next entity
                break;
            }
        }
        return true;
    }

    bool ParseOpenStreetMap(std::shared_ptr<CXMLReader> src){ // combine parse nodes and parse ways
        SXMLEntity TempEntity;
        
        while(src->ReadEntity(TempEntity)){
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DOSMTag){ // osm start
                while(src->ReadEntity(TempEntity)){
                    if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNodeTag){// if node
                        ParseNodes(src, TempEntity); // parse nodes
                        // ParseNodes might have already consumed the start of ways in TempEntity
                        ParseWays(src, TempEntity); // parse ways
                    }
                    else if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag){// if way
                        ParseWays(src, TempEntity); // parse ways
                    }
                    else if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DOSMTag){// osm end
                        return true;
                    }
                }
            }
        }
        return false;
    }

    SImplementation(std::shared_ptr<CXMLReader> src){
        ParseOpenStreetMap(src);
    }

    std::size_t NodeCount() const noexcept{
        return DNodesByIndex.size();
    }

    std::size_t WayCount() const noexcept{
        return DWaysByIndex.size();
    }
    
    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept{
        if(index >= DNodesByIndex.size()){
            return nullptr;
        }
        return DNodesByIndex[index];
    }
    
    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept{ // find node by id
        auto It = DNodesByID.find(id); // find the node in the map
        if(It == DNodesByID.end()){ // if the node is not found
            return nullptr;
        }
        return It->second; // return the node
    }
    
    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept{
        if(index >= DWaysByIndex.size()){ // if index is out of bounds
            return nullptr;
        }
        return DWaysByIndex[index]; // return the way
    }
    
    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept{ // find way by id
        auto It = DWaysByID.find(id); // find the way in the map
        if(It == DWaysByID.end()){
            return nullptr; // if the way is not found
        }
        return It->second; // return the way
    }
};
    
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src){
    DImplementation = std::make_unique<SImplementation>(src);
}

COpenStreetMap::~COpenStreetMap(){

}

std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}
std::size_t COpenStreetMap::WayCount() const noexcept{
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept{
    return DImplementation->NodeByIndex(index);
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept{
    return DImplementation->NodeByID(id);
}
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept{
    return DImplementation->WayByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept{
    return DImplementation->WayByID(id);
}
