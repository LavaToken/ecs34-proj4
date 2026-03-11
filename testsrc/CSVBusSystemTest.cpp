#include <gtest/gtest.h>
#include "CSVBusSystem.h"
#include "StringDataSource.h"

TEST(CSVBusSystem, SimpleFiles)
{
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                              "1,123\n"
                                                              "2,124");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                               "A,1\n"
                                                               "A,2\n"
                                                               "B,1");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 2);
    EXPECT_EQ(BusSystem.RouteCount(), 2);

    auto StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj, nullptr);
    EXPECT_EQ(StopObj->ID(), 1);
    EXPECT_EQ(StopObj->NodeID(), 123);

    auto RouteObj = BusSystem.RouteByName("A");
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->Name(), "A");
    EXPECT_EQ(RouteObj->StopCount(), 2);
    EXPECT_EQ(RouteObj->GetStopID(0), 1);
    EXPECT_EQ(RouteObj->GetStopID(1), 2);
    EXPECT_EQ(RouteObj->GetStopID(2), CBusSystem::InvalidStopID);

    RouteObj = BusSystem.RouteByName("B");
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->Name(), "B");
    EXPECT_EQ(RouteObj->StopCount(), 1);
    EXPECT_EQ(RouteObj->GetStopID(0), 1);

    EXPECT_EQ(BusSystem.RouteByName("C"), nullptr);
}

TEST(CSVBusSystem, EmptyFiles)
{
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 0);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
}

TEST(CSVBusSystem, MalformedCSV)
{
    // Test with non-numeric IDs and missing values
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                              "1,123\n"
                                                              "abc,456\n"
                                                              "2,def\n"
                                                              "3"); // Missing value
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                               "A,1\n"
                                                               "B,xyz");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    // Only the first valid stop (1, 123) should be loaded
    EXPECT_EQ(BusSystem.StopCount(), 1);
    EXPECT_EQ(BusSystem.RouteCount(), 1);
    
    auto RouteObj = BusSystem.RouteByName("A");
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->StopCount(), 1);
}

TEST(CSVBusSystem, OrphanedStopID)
{
    // Test with routes referencing stop IDs that don't exist
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                              "1,123");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                               "A,1\n"
                                                               "A,999"); // 999 doesn't exist
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 1);
    EXPECT_EQ(BusSystem.RouteCount(), 1);

    auto RouteObj = BusSystem.RouteByName("A");
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->StopCount(), 1); // Only stop 1 should be in route A
}

TEST(CSVBusSystem, DuplicateRouteName)
{
    // Test merging rows with the same route name
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                              "1,123\n"
                                                              "2,124");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                               "A,1\n"
                                                               "A,2\n"
                                                               "A,1"); // Duplicate route entry
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.RouteCount(), 1);
    auto RouteObj = BusSystem.RouteByName("A");
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->StopCount(), 3);
}

TEST(CSVBusSystem, OutOfOrderHeaders)
{
    // Test headers in different order
    auto StopDataSource = std::make_shared<CStringDataSource>("node_id,stop_id\n"
                                                              "123,1");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("stop_id,route\n"
                                                               "1,A");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 1);
    EXPECT_EQ(BusSystem.RouteCount(), 1);
    
    auto StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj, nullptr);
    EXPECT_EQ(StopObj->NodeID(), 123);

    auto RouteObj = BusSystem.RouteByName("A");
    ASSERT_NE(RouteObj, nullptr);
}

TEST(CSVBusSystem, Accessors)
{
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                              "1,123\n"
                                                              "2,124");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                               "A,1\n"
                                                               "B,2");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    // StopByIndex
    EXPECT_NE(BusSystem.StopByIndex(0), nullptr);
    EXPECT_NE(BusSystem.StopByIndex(1), nullptr);
    EXPECT_EQ(BusSystem.StopByIndex(2), nullptr);

    // RouteByIndex
    EXPECT_NE(BusSystem.RouteByIndex(0), nullptr);
    EXPECT_NE(BusSystem.RouteByIndex(1), nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(2), nullptr);

    // StopByID
    EXPECT_EQ(BusSystem.StopByID(999), nullptr);

    // RouteByName
    EXPECT_EQ(BusSystem.RouteByName("Invalid"), nullptr);
}

TEST(CSVBusSystem, HeaderErrors)
{
    // Missing stop_id header
    auto StopDataSource1 = std::make_shared<CStringDataSource>("wrong_id,node_id\n1,123");
    auto StopReader1 = std::make_shared<CDSVReader>(StopDataSource1, ',');
    auto RouteDataSource1 = std::make_shared<CStringDataSource>("route,stop_id\nA,1");
    auto RouteReader1 = std::make_shared<CDSVReader>(RouteDataSource1, ',');
    CCSVBusSystem BusSystem1(StopReader1, RouteReader1);
    EXPECT_EQ(BusSystem1.StopCount(), 0);

    // Missing node_id header
    auto StopDataSource2 = std::make_shared<CStringDataSource>("stop_id,wrong_id\n1,123");
    auto StopReader2 = std::make_shared<CDSVReader>(StopDataSource2, ',');
    CCSVBusSystem BusSystem2(StopReader2, RouteReader1);
    EXPECT_EQ(BusSystem2.StopCount(), 0);

    // Missing route header
    auto StopDataSource3 = std::make_shared<CStringDataSource>("stop_id,node_id\n1,123");
    auto StopReader3 = std::make_shared<CDSVReader>(StopDataSource3, ',');
    auto RouteDataSource2 = std::make_shared<CStringDataSource>("wrong_header,stop_id\nA,1");
    auto RouteReader2 = std::make_shared<CDSVReader>(RouteDataSource2, ',');
    CCSVBusSystem BusSystem3(StopReader3, RouteReader2);
    EXPECT_EQ(BusSystem3.RouteCount(), 0);
}

TEST(CSVBusSystem, CompletelyEmpty)
{
    // Completely empty data sources (no headers)
    auto StopDataSource = std::make_shared<CStringDataSource>("");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 0);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
}

TEST(CSVBusSystem, EmptyRouteFile)
{
    // Valid stops, but route file is completely empty (no header)
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n1,123");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 1);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
}

TEST(CSVBusSystem, ShortRouteRow)
{
    // Route row has fewer columns than the header indicates
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n1,123");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                               "A,1\n"
                                                               "B"); // Missing stop_id
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

    EXPECT_EQ(BusSystem.RouteCount(), 1); // Only route A should be loaded
    EXPECT_NE(BusSystem.RouteByName("A"), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("B"), nullptr);
}
