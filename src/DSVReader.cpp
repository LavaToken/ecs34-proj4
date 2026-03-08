#include "DSVReader.h"

#include <iostream>
using std::cout;
using std::endl;

struct CDSVReader::SImplementation
{
    std::shared_ptr<CDataSource> DSource;
    char DDelimiter;

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
    {
        DSource = src;
        DDelimiter = delimiter;
    }

    bool ParseValue(std::string &val)
    {
        bool InQuotes = false;
        val.clear();
        bool charsRead = false;
        while (!DSource->End())
        {
            char NextChar;
            DSource->Peek(NextChar);
            if (!InQuotes && ((NextChar == DDelimiter) || (NextChar == '\n')))
            {
                return true;
            }
            if (NextChar == '\"' && !InQuotes)
            {
                InQuotes = true;
            }
            else
            {
                val += std::string(1, NextChar);
                charsRead = true;
            }
            DSource->Get(NextChar);
        }
        return charsRead || InQuotes;
    }

    bool End() const
    {
        return DSource->End();
    }

    bool ReadRow(std::vector<std::string> &row)
    {
        row.clear();
        if (DSource->End())
            return false;

        while (!DSource->End())
        {
            std::string NextValue;
            ParseValue(NextValue);
            row.push_back(NextValue);

            char NextChar;
            if (DSource->End())
                break;

            DSource->Peek(NextChar);
            if (NextChar == DDelimiter)
            {
                DSource->Get(NextChar); // Consume delimiter
            }
            else if (NextChar == '\n')
            {
                DSource->Get(NextChar); // Consume newline
                return true;
            }
        }
        return !row.empty();
    }
};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
{
    DImplementation = std::make_unique<SImplementation>(src, delimiter);
}

CDSVReader::~CDSVReader()
{
}

bool CDSVReader::End() const
{
    return DImplementation->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row)
{
    return DImplementation->ReadRow(row);
}
