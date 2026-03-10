#include <gtest/gtest.h>
#include "DSVWriter.h"
#include "DSVReader.h"
#include "StringDataSink.h"
#include "StringDataSource.h"

TEST(DSVWriterTest, EmptyRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({}));
    EXPECT_EQ(DataSink->String(),"");

}

TEST(DSVWriterTest, SingleRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"A,B,C");

}

TEST(DSVWriterTest, MultipleRowsTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(), "");
    EXPECT_TRUE(Writer.WriteRow({"1","2","3"}));
    EXPECT_TRUE(Writer.WriteRow({"4","5","6"}));
    EXPECT_TRUE(Writer.WriteRow({"7","8","9"}));
    EXPECT_EQ(DataSink->String(),"1,2,34,5,67,8,9");
}

TEST(DSVWriterTest, EmptyStringTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(), "");
    EXPECT_TRUE(Writer.WriteRow({"hi","","there","crazy","","fox"}));
    EXPECT_EQ(DataSink->String(), "hi,,there,crazy,,fox");
}

TEST(DSVWriterTest, DiffDelimiterTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, '\t');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(), "");
    EXPECT_TRUE(Writer.WriteRow({"Top 3 Things To Do", "1. Eat", "2. Sleep", "3. Eat Again"}));
    EXPECT_EQ(DataSink->String(),"Top 3 Things To Do\t1. Eat\t2. Sleep\t3. Eat Again");
}

TEST(DSVWriterTest, HeckaQuotesTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"I told him, \"No 67 is better than 41\"!", "He replied, \"You are tripping\""}));
    EXPECT_EQ(DataSink->String(),"I told him, \"No 67 is better than 41\"!,He replied, \"You are tripping\"");
}

TEST(DSVWriterTest, NewLineTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"Top 3 Things To Do\n", "1. Eat\n", "2. Sleep\n", "3. Eat Again"}));
    EXPECT_EQ(DataSink->String(),"Top 3 Things To Do\n,1. Eat\n,2. Sleep\n,3. Eat Again");
}

TEST(DSVWriterTest, SpaceTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"   Woah there","calm down   ","  middleman  ","Jake"}));
    EXPECT_EQ(DataSink->String(),"   Woah there,calm down   ,  middleman  ,Jake");
}

// DSVReader Tests
