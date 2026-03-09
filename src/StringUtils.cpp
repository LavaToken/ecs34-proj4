#include "StringUtils.h"
#include <cctype>
#include <algorithm>

namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept{
    std::string temp = str;
    size_t s = start;
    size_t e = end;
    if(!temp.empty()){
        if(s < 0){
            s = 0;
        } else if(s >= temp.length()){
            s = temp.length();
        }
        if(e < 0){
            e = 0;
        } else if(e >= temp.length()){
            e = temp.length();
        }
    }
    return temp.substr(s, e-s);
}

std::string Capitalize(const std::string &str) noexcept{
    // Replace code here
    std::string Temp = str;
    if(!Temp.empty()){
        Temp[0] = toupper(Temp[0]);
        for(size_t Index = 1; Index < Temp.length(); Index++){
            Temp[Index] = tolower(Temp[Index]);
        }
    }
    return Temp;
}

std::string Upper(const std::string &str) noexcept{
    std::string Temp = str;
    if(!Temp.empty()){
        for(size_t i = 0; i < Temp.length(); i++){
            Temp[i] = toupper(Temp[i]);
        }
    }
    return Temp;
}

std::string Lower(const std::string &str) noexcept{
    std::string Temp = str;
    if(!Temp.empty()){
        for(size_t i = 0; i < Temp.length(); i++){
            Temp[i] = tolower(Temp[i]);
        }
    }
    return Temp;
}

std::string LStrip(const std::string &str) noexcept{
    std::string Temp = str;
    size_t Index = 0;
    size_t Length = Temp.length();
    while((Index < Length) && isspace(Temp[Index])){
        Index++;
    }

    return Temp.substr(Index);
}

std::string RStrip(const std::string &str) noexcept{
    std::string Temp = str;
    if(!Temp.empty()){
        size_t Index = Temp.length()-1;
        while(isspace(Temp[Index])){
            if(!Index){
                return "";
            }
            Index--;
        }
        return Temp.substr(0,Index+1);
    }
    return "";
}

std::string Strip(const std::string &str) noexcept{
    return LStrip(RStrip(str));
}

std::string Center(const std::string &str, int width, char fill) noexcept{
    std::string temp = str;
    std::string initial;
    size_t len = temp.length();
    std::string filling;
    filling = fill;
    if(len >= width || width < 0){
        return temp;
    }
    size_t remaining = width - len;
    size_t left = remaining - (remaining / 2);
    size_t right = remaining - left;
    for(size_t i = 0; i < left; i++){
        initial += filling;
    }
    initial += temp;
    for(size_t i = 0; i < right; i++){
        initial += filling;
    }
    return initial; 
}

std::string LJust(const std::string &str, int width, char fill) noexcept{
    std::string temp = str;
    std::string initial;
    size_t len = temp.length();
    if(width < 0 || width < len){
        return temp;
    }
    size_t remaining = width - len;
    std::string filling;
    filling = fill;
    initial += temp;
    for(size_t i = 0; i < remaining; i++){
        initial += filling;
    }
    return initial;
}

std::string RJust(const std::string &str, int width, char fill) noexcept{
    std::string temp = str;
    std::string initial;
    size_t len = temp.length();
    if(width < 0 || width < len){
        return temp;
    }
    size_t remaining = width - len;
    std::string filling;
    filling = fill;
    for(size_t i = 0; i < remaining; i++){
        initial += filling;
    }
    initial += temp;
    return initial;
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept{
    std::string temp = str;
    std::string result;
    size_t oldlen = old.length();
    if(old == ""){
        for(size_t i = 0; i < temp.length(); i++){
            result += rep;
            result += temp[i];
        }
        result += rep;
    }else{
        for(size_t i = 0; i < temp.length(); i++){
            if(temp.substr(i, oldlen) == old){
                result += rep;
                i += oldlen-1;
            }else{
                result += temp[i];
            }
        }
    }
    return result;
}

std::vector< std::string > Split(const std::string &str, const std::string &splt) noexcept{
    std::string temp = str;
    std::vector<std::string> result;
    size_t splitlength = splt.length();
    if(splt == ""){
        for(size_t i = 0; i < temp.length(); i++){
            if(isspace(temp[i])){
                continue;
            }else{
                result.push_back(std::string(1, temp[i]));
            }
        }
    }else if(splitlength >= temp.length()){
        result.push_back(temp);
    }else{
        size_t last = 0;
        for(size_t i = 0; i + splitlength <= temp.length(); i++){
            if(temp.substr(i, splitlength) == splt){
                result.push_back(temp.substr(last, i - last));
                last = i + splitlength;
                i += splitlength - 1;
            }
        }
        result.push_back(temp.substr(last));
    }
    return result;
}

std::string Join(const std::string &str, const std::vector< std::string > &vect) noexcept{
    std::vector<std::string> temp = vect;
    std::string result;
    if(temp.empty()){
        return "";
    }
    if(temp.size() == 1){
        result += temp[0];
    }else{
        for(size_t i = 0; i < temp.size() - 1; i++){
            result += temp[i];
            result += str;
        }
        result += temp[temp.size() - 1];
    }
    return result; // easy
}

std::string ExpandTabs(const std::string &str, int tabsize) noexcept{
    std::string temp = str;
    std::string result;
    std::string tab;
    if(tabsize <= 0){
        tab = "";
    }else{
        for(size_t i = 0; i < tabsize; i++){
            tab += " ";
        }
    }
    for(size_t i = 0; i < temp.length(); i++){
        if(temp.substr(i, 1) == "\t"){
            result += tab;
        }else{
            result += temp.substr(i, 1);
        }
    }
    return result;
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept{
    // Map implementation?
    // Influence on Wikipedia pseudocode provided by Instructor
    size_t x = left.length();
    size_t y = right.length();
    std::vector<std::vector<size_t>> dyna(x + 1, std::vector<size_t>(y + 1, 0)); // This is so much easier in java :')
    size_t cost;
    for(size_t i = 0; i <= x; i++){
        dyna[i][0] = i;
    }
    for(size_t j = 0; j <= y; j++){
        dyna[0][j] = j;
    }
    if(!ignorecase){
        for(size_t j = 1; j <= y; j++){
            for(size_t i = 1; i <= x; i++){
                if(left[i-1] == right[j-1]){
                    cost = 0;
                }else{
                    cost = 1;
                }
                dyna[i][j] = std::min<size_t>({dyna[i-1][j] + 1, //Delete
                                    dyna[i][j-1] + 1,      //Insert
                                    dyna[i-1][j-1] + cost  //Sub
                                    });
            }
        }
    }else{
        for(size_t j = 1; j <= y; j++){
            for(size_t i = 1; i <= x; i++){
                if(tolower(left[i-1]) == tolower(right[j-1])){
                    cost = 0;
                }else{
                    cost = 1;
                }
                dyna[i][j] = std::min<size_t>({dyna[i-1][j] + 1, //Delete
                                    dyna[i][j-1] + 1,      //Insert
                                    dyna[i-1][j-1] + cost  //Sub
                                    });
            }
        }
    }
    return dyna[x][y];
}

}