#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StringDataSource.h"

TEST(OSMTest, SimpleFiles){ // Provided Test Case
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"258592863\"/>\n"
		                                                        "    <nd ref=\"4399281377\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),2);
    EXPECT_EQ(OpenStreetMap.WayCount(),1);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0),nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2),nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1),nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),1);
    EXPECT_EQ(TempNode->Location(),CStreetMap::SLocation(38.5,-121.7));

}

TEST(OSMTest, ObscureFiles){ // Based on provided test case
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"258592863\"/>\n"
		                                                        "    <nd ref=\"4399281377\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),2);  
    EXPECT_EQ(OpenStreetMap.WayCount(),1); // my way or the highway
    EXPECT_NE(OpenStreetMap.NodeByIndex(0),nullptr); // you should exist
    EXPECT_NE(OpenStreetMap.NodeByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2),nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1),nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),1);
    EXPECT_EQ(TempNode->Location(),CStreetMap::SLocation(38.5,-121.7));
}

TEST(OSMTest, EmptyFiles){
    auto OSMDataSource = std::make_shared< CStringDataSource >("");
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),0);
    EXPECT_EQ(OpenStreetMap.WayCount(),0);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0),nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByID(1),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(1),nullptr);
}

TEST(OSMTest, NoNodesOrWaysFile){ // valid xml. but nothing there
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),0); // nope
    EXPECT_EQ(OpenStreetMap.WayCount(),0); // double nope
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByID(1), nullptr);
}

TEST(OSMTest, NodesOnlyFile){ // no roads. gotta carve a path
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <node id=\"10\" lat=\"38.10\" lon=\"-121.10\"/>\n"
                                                                "  <node id=\"20\" lat=\"38.20\" lon=\"-121.20\"/>\n"
                                                                "  <node id=\"30\" lat=\"38.30\" lon=\"-121.30\"/>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),3);
    EXPECT_EQ(OpenStreetMap.WayCount(),0);

    auto node0 = OpenStreetMap.NodeByIndex(0);
    auto node1 = OpenStreetMap.NodeByIndex(1);
    auto node2 = OpenStreetMap.NodeByIndex(2);
    ASSERT_NE(node0, nullptr);
    ASSERT_NE(node1, nullptr);
    ASSERT_NE(node2, nullptr);

    EXPECT_EQ(OpenStreetMap.NodeByIndex(3), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);

    EXPECT_NE(OpenStreetMap.NodeByID(10), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByID(20), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByID(30), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByID(676), nullptr); // haha 67
}

TEST(OSMTest, WaysOnlyFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8' ?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <way id=\"100\">\n"
                                                                "    <nd ref=\"1\"/>\n"
                                                                "    <nd ref=\"2\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),0);
    EXPECT_EQ(OpenStreetMap.WayCount(),1);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
}

TEST(OSMTest, DuplicateNodesFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <node id=\"5\" lat=\"38.0\" lon=\"-121.0\"/>\n"
                                                                "  <node id=\"5\" lat=\"39.0\" lon=\"-122.0\"/>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_GE(OpenStreetMap.NodeCount(), 1);
    auto id = OpenStreetMap.NodeByID(5);
    ASSERT_NE(id, nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByID(id->ID()), id);
}

TEST(OSMTest, ExactValuesFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <node id=\"1\" lat=\"0.0\" lon=\"0.0\"/>\n"
                                                                "  <node id=\"2\" lat=\"-38.1234\" lon=\"121.9876\"/>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    auto node1 = OpenStreetMap.NodeByID(1);
    auto node2 = OpenStreetMap.NodeByID(2);
    ASSERT_NE(node1, nullptr);
    ASSERT_NE(node2, nullptr);
    EXPECT_EQ(node1->Location(), CStreetMap::SLocation(0.0, 0.0));
    EXPECT_EQ(node2->Location(), CStreetMap::SLocation(-38.1234, 121.9876));
}

TEST(OSMTest, LotsOfWhiteSpace){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8' ?>\n"
                                                    "\n\n   <osm version=\"0.6\" generator=\"unit_test\">\n"
                                                    "   \n"
                                                    "     <node id=\"1\" lat=\"38.5\" lon=\"-121.7\" />   \n"
                                                    "\n"
                                                    "     <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                    "     \n"
                                                    "     <way id=\"100\" >\n"
                                                    "       <nd ref=\"1\"/>\n"
                                                    "       <nd ref=\"2\"/>\n"
                                                    "     </way>\n"
                                                    "   \n"
                                                    " </osm>\n"
                                                    );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 2);
    EXPECT_EQ(OpenStreetMap.WayCount(), 1);
}

TEST(OSMTest, NodeAttributes){ 
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8' ?>\n"
                                                    "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                    "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                    "    <tag k=\"highway\" v=\"crossing\"/>\n"
                                                    "  </node>\n"
                                                    "</osm>\n"
                                                    );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    
    auto node = OpenStreetMap.NodeByID(1);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->HasAttribute("highway")); // Attribute Exists
    EXPECT_EQ(node->GetAttribute("highway"), "crossing"); // Wats the attribute?
    EXPECT_EQ(node->AttributeCount(), 1); // How many attributes? Should be 1
    EXPECT_EQ(node->GetAttributeKey(0), "highway");
    EXPECT_EQ(node->GetAttributeKey(1), "");
}

TEST(OSMTest, WayAttributes){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8' ?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <way id=\"100\">\n"
                                                                "    <tag k=\"highway\" v=\"crossing\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    auto way = OpenStreetMap.WayByID(100);
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->NodeCount(), 0); // No nodes in way
    EXPECT_EQ(way->GetAttribute("highway"), "crossing");
    EXPECT_EQ(way->GetAttribute("missing"), "");
    EXPECT_EQ(way->GetAttributeKey(way->AttributeCount()), "");
}

TEST(OSMTest, SelfInteractFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8' ?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <node id=\"1\" lat=\"0\" lon=\"0\"/>\n"
                                                                "  <node id=\"2\" lat=\"0\" lon=\"1\"/>\n"
                                                                "  <node id=\"3\" lat=\"1\" lon=\"1\"/>\n"
                                                                "  <node id=\"4\" lat=\"1\" lon=\"0\"/>\n"
                                                                "  <way id=\"200\">\n"
                                                                "    <nd ref=\"1\"/>\n"
                                                                "    <nd ref=\"3\"/>\n"
                                                                "    <nd ref=\"2\"/>\n"
                                                                "    <nd ref=\"4\"/>\n"
                                                                "    <nd ref=\"1\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    auto way = OpenStreetMap.WayByID(200);
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->NodeCount(), 5);
    EXPECT_EQ(way->GetNodeID(0), 1);
    EXPECT_EQ(way->GetNodeID(4), 1);
}

TEST(OSMTest, NoTagsFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <way id=\"100\">\n"
                                                                "    <tag k=\"highway\"/>\n"              // no v
                                                                "    <tag v=\"crossing\"/>\n"             // no k
                                                                "    <tag k=\"weird:key\" v=\"123\"/>\n"  // not normal 
                                                                "  </way>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    auto way = OpenStreetMap.WayByID(100);
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->GetAttribute("highway"), "");
    EXPECT_EQ(way->GetAttribute("weird:key"), "123");
}

TEST(OSMTest, DuplicateIDFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <way id=\"300\"><nd ref=\"1\"/></way>\n"
                                                                "  <way id=\"300\"><nd ref=\"2\"/></way>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    auto way = OpenStreetMap.WayByID(300);
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->ID(), 300);
}

TEST(OSMTest, DirectionIssueFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"unit_test\">\n"
                                                                "  <way id=\"600\">\n"
                                                                "    <nd ref=\"10\"/>\n"
                                                                "    <nd ref=\"20\"/>\n"
                                                                "    <nd ref=\"30\"/>\n"
                                                                "    <tag k=\"oneway\" v=\"yes\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>\n"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    auto way = OpenStreetMap.WayByID(600);
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->GetNodeID(0), 10);
    EXPECT_EQ(way->GetNodeID(1), 20);
    EXPECT_EQ(way->GetNodeID(2), 30);
}