#include <gtest/gtest.h>
#include "StringUtils.h"

TEST(StringUtilsTest, Slice){
    EXPECT_EQ(StringUtils::Slice("",0,0),std::string(""));
    EXPECT_EQ(StringUtils::Slice("quickbrownfox",0,5),std::string("quick"));
    EXPECT_EQ(StringUtils::Slice("quickbrownfox",4, 12),std::string("kbrownfo"));
}

TEST(StringUtilsTest, Capitalize){
    EXPECT_EQ(StringUtils::Capitalize("hello"),std::string("Hello"));
    EXPECT_EQ(StringUtils::Capitalize("HELLO"),std::string("Hello"));
    EXPECT_EQ(StringUtils::Capitalize(" hello"),std::string(" hello"));
    EXPECT_EQ(StringUtils::Capitalize("HELLO WORLD"),std::string("Hello world"));
    EXPECT_EQ(StringUtils::Capitalize("hello world"),std::string("Hello world"));
    EXPECT_EQ(StringUtils::Capitalize(" "),std::string(" "));
}

TEST(StringUtilsTest, Upper){
    EXPECT_EQ(StringUtils::Upper("heya"),std::string("HEYA"));
    EXPECT_EQ(StringUtils::Upper("HeYa"),std::string("HEYA"));
    EXPECT_EQ(StringUtils::Upper("@12Heya93"),std::string("@12HEYA93"));
    EXPECT_EQ(StringUtils::Upper("hEYa\n"),std::string("HEYA\n"));
    EXPECT_EQ(StringUtils::Upper(""),std::string(""));
}

TEST(StringUtilsTest, Lower){
    EXPECT_EQ(StringUtils::Lower("HEYYA"),std::string("heyya"));
    EXPECT_EQ(StringUtils::Lower("HeYa"),std::string("heya"));
    EXPECT_EQ(StringUtils::Lower("@12HeYYa93"),std::string("@12heyya93"));
    EXPECT_EQ(StringUtils::Lower("hEYa\n"),std::string("heya\n"));
    EXPECT_EQ(StringUtils::Lower(""),std::string(""));
}

TEST(StringUtilsTest, LStrip){
    EXPECT_EQ(StringUtils::LStrip("   howdy  "),std::string("howdy  "));
    EXPECT_EQ(StringUtils::LStrip("Hello "),std::string("Hello "));
    EXPECT_EQ(StringUtils::LStrip(" Hello"),std::string("Hello"));
    EXPECT_EQ(StringUtils::LStrip("\n  Howdy "),std::string("Howdy "));
}

TEST(StringUtilsTest, RStrip){
    EXPECT_EQ(StringUtils::RStrip("   howdy  "),std::string("   howdy"));
    EXPECT_EQ(StringUtils::RStrip("Hello "),std::string("Hello"));
    EXPECT_EQ(StringUtils::RStrip("Hello    "),std::string("Hello"));
    EXPECT_EQ(StringUtils::RStrip("  Howdy\n"),std::string("  Howdy"));
}

TEST(StringUtilsTest, Strip){
    EXPECT_EQ(StringUtils::Strip("   howdy  "),std::string("howdy"));
    EXPECT_EQ(StringUtils::Strip("  "),std::string(""));
    EXPECT_EQ(StringUtils::Strip(""),std::string(""));
    EXPECT_EQ(StringUtils::Strip("   howdy  "),std::string("howdy"));
    EXPECT_EQ(StringUtils::Strip("   howdy  "),std::string("howdy"));
    EXPECT_EQ(StringUtils::Strip("\n Howdy \t"),std::string("Howdy"));
    EXPECT_EQ(StringUtils::Strip(""),std::string(""));
    EXPECT_EQ(StringUtils::Strip("\n\t\n "),std::string(""));
}

TEST(StringUtilsTest, Center){
    EXPECT_EQ(StringUtils::Center("hi",6),std::string("  hi  "));
    EXPECT_EQ(StringUtils::Center("hi",5),std::string("  hi ")); // Super weird test case
    EXPECT_EQ(StringUtils::Center("money",7,'$'),std::string("$money$"));
    EXPECT_EQ(StringUtils::Center("over",2,'*'),std::string("over"));
    EXPECT_EQ(StringUtils::Center("hi",-5),std::string("hi"));

}

TEST(StringUtilsTest, LJust){
    EXPECT_EQ(StringUtils::LJust("mu",3),std::string("mu "));
    EXPECT_EQ(StringUtils::LJust("mu",5),std::string("mu   "));
    EXPECT_EQ(StringUtils::LJust("mu",2),std::string("mu"));
    EXPECT_EQ(StringUtils::LJust("mu",1),std::string("mu"));
    EXPECT_EQ(StringUtils::LJust("mu",-2),std::string("mu"));
    EXPECT_EQ(StringUtils::LJust("mu",5,'#'),std::string("mu###"));
}

TEST(StringUtilsTest, RJust){
    EXPECT_EQ(StringUtils::RJust("mu",3),std::string(" mu"));
    EXPECT_EQ(StringUtils::RJust("mu",5),std::string("   mu"));
    EXPECT_EQ(StringUtils::RJust("mu",2),std::string("mu"));
    EXPECT_EQ(StringUtils::RJust("mu",1),std::string("mu"));
    EXPECT_EQ(StringUtils::RJust("mu",-2),std::string("mu"));
    EXPECT_EQ(StringUtils::RJust("mu",5,'#'),std::string("###mu"));
}

TEST(StringUtilsTest, Replace){
    EXPECT_EQ(StringUtils::Replace("the quick brown fox jumped over the lazy dog","the","no"),std::string("no quick brown fox jumped over no lazy dog"));
    EXPECT_EQ(StringUtils::Replace("banana","a","e"),std::string("benene"));
    EXPECT_EQ(StringUtils::Replace("aaaaaa","aa","z"),std::string("zzz"));
    EXPECT_EQ(StringUtils::Replace("aaaaa","aa","z"),std::string("zza"));
    EXPECT_EQ(StringUtils::Replace("xyz","","$"),std::string("$x$y$z$"));
}

TEST(StringUtilsTest, Split){
    EXPECT_EQ(StringUtils::Split("the,quick,brown,fox",","),std::vector<std::string>({"the","quick","brown","fox"}));
    EXPECT_EQ(StringUtils::Split(" the, quick, brown, fox",","),std::vector<std::string>({" the"," quick"," brown"," fox"}));
    EXPECT_EQ(StringUtils::Split("a b\tc\n\td"),std::vector<std::string>({"a","b","c","d"}));
}

TEST(StringUtilsTest, Join){
    EXPECT_EQ(StringUtils::Join(" ",std::vector<std::string>({"the","quick","brown","fox"})),std::string("the quick brown fox"));
    EXPECT_EQ(StringUtils::Join(",",std::vector<std::string>({"hello", "world"})),std::string("hello,world"));
    EXPECT_EQ(StringUtils::Join("hello",std::vector<std::string>({"what"})),std::string("what"));
    EXPECT_EQ(StringUtils::Join(",",std::vector<std::string>({"hello", " ", "world"})),std::string("hello, ,world"));
}

TEST(StringUtilsTest, ExpandTabs){
    EXPECT_EQ(StringUtils::ExpandTabs("the\tquick\tbrown\tfox",1),std::string("the quick brown fox"));
    EXPECT_EQ(StringUtils::ExpandTabs("the\tquick\tbrown\tfox",4),std::string("the    quick    brown    fox"));
}

TEST(StringUtilsTest, EditDistance){
    EXPECT_EQ(StringUtils::EditDistance("",""),0);
    EXPECT_EQ(StringUtils::EditDistance("", "hey"),3);
    EXPECT_EQ(StringUtils::EditDistance("hey", ""),3);
    EXPECT_EQ(StringUtils::EditDistance("Racecar","racecar", true),0);
    EXPECT_EQ(StringUtils::EditDistance("Racecar","racecar", false),1);
    EXPECT_EQ(StringUtils::EditDistance("kitten","sitting"),3);
}
