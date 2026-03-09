#include <gtest/gtest.h>
#include "DijkstraPathRouter.h"

TEST(DijkstraPathRouter, SimpleTest){
    CDijkstraPathRouter PathRouter;

    EXPECT_EQ(PathRouter.VertexCount(),0);
    auto VertexID = PathRouter.AddVertex(std::string("foo"));
    EXPECT_EQ(PathRouter.VertexCount(),1);
    auto VetextTag = std::any_cast<std::string>(PathRouter.GetVertexTag(VertexID));
    EXPECT_EQ(VetextTag,"foo");
    
}

TEST(DijkstraPathRouter, ShortestPath){
    CDijkstraPathRouter PathRouter;
    /*
                5   
       A ---> B --->C
          4  2|  ___^
              V / 1
              D
    
    */
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(),4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,4.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexC,5.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexD,2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD,VertexC,1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexC,Path), 7.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexB,VertexD,VertexC};
    EXPECT_EQ(Path,ExpectedPath);
    
}

// Single Node Test
TEST(DijkstraPathRouter, SingleNodeTest){
    CDijkstraPathRouter PathRouter;
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    EXPECT_EQ(PathRouter.VertexCount(),1);
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexA,Path), 0.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA};
    EXPECT_EQ(Path,ExpectedPath);
}

// Disconnected Graph
TEST(DijkstraPathRouter, DisconnectedGraphTest){
    CDijkstraPathRouter PathRouter;
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    EXPECT_EQ(PathRouter.VertexCount(),2);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexB,Path), 1.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexB};
    EXPECT_EQ(Path,ExpectedPath);
}

// With Cycles
// TEST(DijkstraPathRouter, CycleTest){
//     CDijkstraPathRouter PathRouter;
//     auto VertexA = PathRouter.AddVertex(std::string("A"));
//     auto VertexB = PathRouter.AddVertex(std::string("B"));
//     EXPECT_EQ(PathRouter.VertexCount(),2);
//     EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,1.0));
//     EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexA,1.0));
//     std::vector<CPathRouter::TVertexID> Path;
//     EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexB,Path), 1.0);
//     std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexB};
//     EXPECT_EQ(Path,ExpectedPath);
// }

// More than one shortest path
/*A --1--> B
A --2--> C
B --1--> C*/
TEST(DijkstraPathRouter, MultipleShortestPathTest){
    CDijkstraPathRouter PathRouter;
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    EXPECT_EQ(PathRouter.VertexCount(),3);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexC,2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexC,1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexC,Path), 2.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexB,VertexC};
    EXPECT_EQ(Path,ExpectedPath);
}

// Worse direct path
/*A --10--> B
A --1--> C
C --1--> B*/
TEST(DijkstraPathRouter, WorseDirectPathTest){
    CDijkstraPathRouter PathRouter;
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    EXPECT_EQ(PathRouter.VertexCount(),3);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,10.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexC,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexC,VertexB,1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexB,Path), 1.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexC,VertexB};
    EXPECT_EQ(Path,ExpectedPath);
}

// Zero Weight Edges
/*A --0--> B*/
TEST(DijkstraPathRouter, ZeroWeightEdgesTest){
    CDijkstraPathRouter PathRouter;
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    EXPECT_EQ(PathRouter.VertexCount(),2);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,0.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexB,Path), 0.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexB};
    EXPECT_EQ(Path,ExpectedPath);
}

// Negative Weight Edges
/*A --(-1)--> B*/
TEST(DijkstraPathRouter, NegativeWeightEdgesTest){
    CDijkstraPathRouter PathRouter;
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    EXPECT_EQ(PathRouter.VertexCount(),2);
    EXPECT_FALSE(PathRouter.AddEdge(VertexA,VertexB,-1.0));
} 
