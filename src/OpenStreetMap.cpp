#include "OpenStreetMap.h"
#include <unordered_map>
#include <vector>

struct COpenStreetMap::SImplementation{
    const std::string DOSMTag = "osm";
    const std::string DNodeTag = "node";
    const std::string DNodeIDAttr = "id";
    const std::string DNodeLatAttr = "lat";
    const std::string DNodeLonAttr = "lon";
    const std::string DWayTag = "way";
    const std::string DWayIDAttr = "id";
    const std::string DTagTag = "tag";
    const std::string DTagKAttr = "k";
    const std::string DTagVAttr = "v";
    const std::string DWayNdTag = "nd";
    const std::string DWayNdRefAttr = "ref";

    struct SNode: public CStreetMap::SNode{
        TNodeID DID;
        SLocation DLocation;
        std::vector<std::pair<std::string, std::string>> DAttributes;
        
        ~SNode(){};
        TNodeID ID() const noexcept override{
            return DID;
        }

        SLocation Location() const noexcept override{
            return DLocation;
        }

        std::size_t AttributeCount() const noexcept override{
            return DAttributes.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if (index < DAttributes.size()) {
                return DAttributes[index].first;
            }
            return "";
        }

        bool HasAttribute(const std::string &key) const noexcept override{
            for (const auto& attr : DAttributes) {
                if (attr.first == key) return true;
            }
            return false;
        }

        std::string GetAttribute(const std::string &key) const noexcept override{
            for (const auto& attr : DAttributes) {
                if (attr.first == key) return attr.second;
            }
            return "";
        }
    };

    struct SWay: public CStreetMap::SWay{
        TWayID DID;
        std::vector<TNodeID> DNodeIDs;
        std::vector<std::pair<std::string, std::string>> DAttributes;
        
        ~SWay(){};
        TWayID ID() const noexcept override{
            return DID;
        }

        std::size_t NodeCount() const noexcept override{
            return DNodeIDs.size();
        }

        TNodeID GetNodeID(std::size_t index) const noexcept override{
            if (index < DNodeIDs.size()) {
                return DNodeIDs[index];
            }
            return CStreetMap::InvalidNodeID;
        }

        std::size_t AttributeCount() const noexcept override{
            return DAttributes.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if (index < DAttributes.size()) {
                return DAttributes[index].first;
            }
            return "";
        }

        bool HasAttribute(const std::string &key) const noexcept override{
            for (const auto& attr : DAttributes) {
                if (attr.first == key) return true;
            }
            return false;
        }

        std::string GetAttribute(const std::string &key) const noexcept override{
            for (const auto& attr : DAttributes) {
                if (attr.first == key) return attr.second;
            }
            return "";
        }
    };
    std::vector<std::shared_ptr<SNode>> DNodesByIndex;
    std::unordered_map<TNodeID,std::shared_ptr<SNode>> DNodesByID;
    std::vector<std::shared_ptr<SWay>> DWaysByIndex;
    std::unordered_map<TWayID,std::shared_ptr<SWay>> DWaysByID;

    bool ParseOpenStreetMap(std::shared_ptr<CXMLReader> src){
        SXMLEntity TempEntity;
        
        if(src->ReadEntity(TempEntity)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement || TempEntity.DType == SXMLEntity::EType::CompleteElement) && TempEntity.DNameData == DOSMTag){
                bool is_osm_complete = (TempEntity.DType == SXMLEntity::EType::CompleteElement);
                if (is_osm_complete) {
                    return true;
                }
                while (src->ReadEntity(TempEntity)) {
                    if (TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DOSMTag) {
                        break;
                    }
                    if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNodeTag) {
                        auto NodeID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr));
                        auto NodeLat = std::stod(TempEntity.AttributeValue(DNodeLatAttr));
                        auto NodeLon = std::stod(TempEntity.AttributeValue(DNodeLonAttr));
                        auto NewNode = std::make_shared<SNode>();
                        NewNode->DID = NodeID;
                        NewNode->DLocation = {NodeLat,NodeLon};
                        DNodesByIndex.push_back(NewNode);
                        DNodesByID[NodeID] = NewNode;
                        while(src->ReadEntity(TempEntity)){
                            if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DNodeTag){
                                break;
                            }
                            if (TempEntity.DType == SXMLEntity::EType::CompleteElement && TempEntity.DNameData == DNodeTag) { // Some parsers might issue complete
                                break;
                            }
                            if ((TempEntity.DType == SXMLEntity::EType::StartElement || TempEntity.DType == SXMLEntity::EType::CompleteElement) && TempEntity.DNameData == DTagTag) {
                                NewNode->DAttributes.push_back({TempEntity.AttributeValue(DTagKAttr), TempEntity.AttributeValue(DTagVAttr)});
                            }
                        }
                    } else if (TempEntity.DType == SXMLEntity::EType::CompleteElement && TempEntity.DNameData == DNodeTag) {
                        auto NodeID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr));
                        auto NodeLat = std::stod(TempEntity.AttributeValue(DNodeLatAttr));
                        auto NodeLon = std::stod(TempEntity.AttributeValue(DNodeLonAttr));
                        auto NewNode = std::make_shared<SNode>();
                        NewNode->DID = NodeID;
                        NewNode->DLocation = {NodeLat,NodeLon};
                        DNodesByIndex.push_back(NewNode);
                        DNodesByID[NodeID] = NewNode;
                    } else if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag) {
                        auto WayID = std::stoull(TempEntity.AttributeValue(DWayIDAttr));
                        auto NewWay = std::make_shared<SWay>();
                        NewWay->DID = WayID;
                        DWaysByIndex.push_back(NewWay);
                        DWaysByID[WayID] = NewWay;
                        while(src->ReadEntity(TempEntity)){
                            if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DWayTag){
                                break;
                            }
                            if ((TempEntity.DType == SXMLEntity::EType::StartElement || TempEntity.DType == SXMLEntity::EType::CompleteElement) && TempEntity.DNameData == DTagTag) {
                                NewWay->DAttributes.push_back({TempEntity.AttributeValue(DTagKAttr), TempEntity.AttributeValue(DTagVAttr)});
                            } else if ((TempEntity.DType == SXMLEntity::EType::StartElement || TempEntity.DType == SXMLEntity::EType::CompleteElement) && TempEntity.DNameData == DWayNdTag) {
                                NewWay->DNodeIDs.push_back(std::stoull(TempEntity.AttributeValue(DWayNdRefAttr)));
                            }
                        }
                    } else if (TempEntity.DType == SXMLEntity::EType::CompleteElement && TempEntity.DNameData == DWayTag) {
                        auto WayID = std::stoull(TempEntity.AttributeValue(DWayIDAttr));
                        auto NewWay = std::make_shared<SWay>();
                        NewWay->DID = WayID;
                        DWaysByIndex.push_back(NewWay);
                        DWaysByID[WayID] = NewWay;
                    }
                }
                return true;
            } else if (TempEntity.DType == SXMLEntity::EType::CompleteElement && TempEntity.DNameData == DOSMTag) {
                return true;
            }
        }
        return false;
    }

    SImplementation(std::shared_ptr<CXMLReader> src){
        if(ParseOpenStreetMap(src)){

        }
    }

    std::size_t NodeCount() const noexcept{
        return DNodesByIndex.size();
    }

    std::size_t WayCount() const noexcept{
        return DWaysByIndex.size();
    }
    
    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept{
        if (index < DNodesByIndex.size()) {
            return DNodesByIndex[index];
        }
        return nullptr;
    }
    
    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept{
        auto it = DNodesByID.find(id);
        if (it != DNodesByID.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept{
        if (index < DWaysByIndex.size()) {
            return DWaysByIndex[index];
        }
        return nullptr;
    }
    
    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept{
        auto it = DWaysByID.find(id);
        if (it != DWaysByID.end()) {
            return it->second;
        }
        return nullptr;
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
