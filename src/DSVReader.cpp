#include "DSVReader.h"

struct CDSVReader::SImplementation{
    std::shared_ptr<CDataSource> DSource;
    char DDelimiter;

    SImplementation(std::shared_ptr< CDataSource > src, char delimiter){
        DSource = src;
        DDelimiter = delimiter;
    }

    bool ParseValue(std::string &val){
        bool InQuotes = false;
        val.clear();

        while(!DSource->End()){
            char NextChar;
            DSource->Peek(NextChar);
            if(!InQuotes &&((NextChar == DDelimiter)||(NextChar == '\n'))){
                return true;
            }
            DSource->Get(NextChar); // Actually consume the character
            if(NextChar == '\"'){
                if(InQuotes){
                    char PeekChar;
                    if(!DSource->End() && DSource->Peek(PeekChar) && (PeekChar == '\"')){
                        DSource->Get(NextChar); // Consume second quote
                        val += '\"';
                    }
                    else{
                        InQuotes = false;
                    }
                }
                else{
                    InQuotes = true;
                }
            }
            else{
                val += NextChar;
            }
        }
        return !val.empty() || InQuotes; // Return true if we read something or were in quotes
    }

    bool End() const{
        return DSource->End();
    }

    bool ReadRow(std::vector<std::string> &row){
        row.clear();
        while(!DSource->End()){
            std::string NextValue;
            if(ParseValue(NextValue)){
                row.push_back(NextValue);
                if(DSource->End()){
                    return true;
                }
                char NextChar;
                DSource->Peek(NextChar);
                if(NextChar == '\n'){
                    DSource->Get(NextChar); // Consume newline
                    return true;
                }
                else if(NextChar == DDelimiter){
                    DSource->Get(NextChar); // Consume delimiter
                }
            }
            else{
                break;
            }
        }
        return !row.empty();
    }

};
        
CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter){
    DImplementation = std::make_unique<SImplementation>(src,delimiter);
}

CDSVReader::~CDSVReader(){

}

bool CDSVReader::End() const{
    return DImplementation->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row){
    return DImplementation->ReadRow(row);
}
