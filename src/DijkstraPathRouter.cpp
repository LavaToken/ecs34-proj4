#include "DijkstraPathRouter.h"
struct CDijkstraPathRouter::SImplementation{
    struct SVertex;
    using TEdge = std::pair<double,std::shared_ptr<SVertex>>;
    struct SVertex{
        std::vector<TEdge> DEdges;
        std::any DTag;
    };
    std::vector<std::shared_ptr<SVertex>> DVertices;

    SImplementation(){

    }
    ~SImplementation(){

    }

    std::size_t VertexCount() const noexcept{
        return DVertices.size();
    }

    TVertexID AddVertex(std::any tag) noexcept{
        auto NewVertex = std::make_shared<SVertex>();
        NewVertex->DTag = tag;
        TVertexID NewID = DVertices.size();
        DVertices.push_back(NewVertex);
        return NewID;
    }

    std::any GetVertexTag(TVertexID id) const noexcept{
        if(id < DVertices.size()){
            return DVertices[id]->DTag;
        }
        return std::any();
    }
    // Discussion code has some issues.. did not detect negative weight edges i think..
    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept{
        if(weight < 0){
            return false;
        }
        if(src < DVertices.size() && dest < DVertices.size()){
            DVertices[src]->DEdges.push_back(std::make_pair(weight,DVertices[dest]));
            if(bidir){
                DVertices[dest]->DEdges.push_back(std::make_pair(weight,DVertices[src]));
            }
            return true;
        }
        return false;
    }

    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept{
        return true;
    }

    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
        std::vector<double> Weights; 
        Weights.resize(DVertices.size(),std::numeric_limits<double>::max());
        std::vector<TVertexID> Previous;
        Previous.resize(DVertices.size(),std::numeric_limits<TVertexID>::max());

        if(src >= DVertices.size() || dest >= DVertices.size()){
            return NoPathExists;
        }

        Weights[src] = 0;
        Previous[src] = InvalidVertexID;
        
        for(std::size_t i = 0; i < DVertices.size(); i++){
            if(i != src){
                Weights[i] = std::numeric_limits<double>::max(); // Infinity
                Previous[i] = InvalidVertexID; // Invalid/Undefined
            }
        }

        while(true){
            TVertexID u = InvalidVertexID;
            double minWeight = std::numeric_limits<double>::max();
            for(std::size_t i = 0; i < DVertices.size(); i++){
                if(Weights[i] < minWeight){
                    minWeight = Weights[i];
                    u = i;
                }
            }
            if(u == InvalidVertexID){
                break;
            }
            for(const auto &edge : DVertices[u]->DEdges){
                // edge.second is a shared_ptr<SVertex>
                auto it = std::find(DVertices.begin(), DVertices.end(), edge.second); // find its index in DVertices
                if(it == DVertices.end()){
                    continue;
                }
                TVertexID v = static_cast<TVertexID>(std::distance(DVertices.begin(), it));
                double alt = Weights[u] + edge.first;
                if(alt <= Weights[v]){
                    Weights[v] = alt;
                    Previous[v] = u;
                }
            }
            if(u == dest){
                break;
            }
            // Mark u as visited(infinity to prevent picked again)
            Weights[u] = std::numeric_limits<double>::max();
        }

        // If destination impossible.
        if(Weights[dest] == std::numeric_limits<double>::max()){
            return NoPathExists;
        }
        // Non trivial, require that destination has a predecessor.
        if(dest != src && Previous[dest] == InvalidVertexID){
            return NoPathExists;
        }
        path.clear();
        for(TVertexID v = dest; v != InvalidVertexID; v = Previous[v]){
            path.push_back(v);
        }
        std::reverse(path.begin(), path.end());
        return Weights[dest];
        
    }
};

CDijkstraPathRouter::CDijkstraPathRouter(){
    DImplementation = std::make_unique<SImplementation>();
}

CDijkstraPathRouter::~CDijkstraPathRouter(){ 
    DImplementation.reset();
}

std::size_t CDijkstraPathRouter::VertexCount() const noexcept{
    return DImplementation->VertexCount();
}

CPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept{
    return DImplementation->AddVertex(tag);
}

std::any CDijkstraPathRouter::GetVertexTag(TVertexID id) const noexcept{
    return DImplementation->GetVertexTag(id);
}

bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept{
    return DImplementation->AddEdge(src,dest,weight,bidir);
}

bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept{
    return DImplementation->Precompute(deadline);
}

double CDijkstraPathRouter::FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
    return DImplementation->FindShortestPath(src,dest,path);
}
