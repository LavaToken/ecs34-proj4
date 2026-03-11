#include "TransportationPlannerCommandLine.h"
#include "StringUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

struct CTransportationPlannerCommandLine::SImplementation{
    std::shared_ptr<CDataSource> DCmdSrc;
    std::shared_ptr<CDataSink> DOutSink;
    std::shared_ptr<CDataSink> DErrSink;
    std::shared_ptr<CDataFactory> DResults;
    std::shared_ptr<CTransportationPlanner> DPlanner;

    SImplementation(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner){
        DCmdSrc = cmdsrc;
        DOutSink = outsink;
        DErrSink = errsink;
        DResults = results;
        DPlanner = planner;
    }

    ~SImplementation(){

    }

    void PrintHelp(){
        std::string HelpText = "------------------------------------------------------------------------\n"
                               "help     Display this help menu\n"
                               "exit     Exit the program\n"
                               "count    Output the number of nodes in the map\n"
                               "node     Syntax \"node [0, count)\" \n"
                               "         Will output node ID and Lat/Lon for node\n"
                               "fastest  Syntax \"fastest start end\" \n"
                               "         Calculates the time for fastest path from start to end\n"
                               "shortest Syntax \"shortest start end\" \n"
                               "         Calculates the distance for the shortest path from start to end\n"
                               "save     Saves the last calculated path to file\n"
                               "print    Prints the steps for the last calculated path\n";
        auto vec = std::vector<char>(HelpText.begin(), HelpText.end());
        DOutSink->Write(vec);
    }

    bool ProcessCommands(){
        std::string OutPrompt = "> ";
        auto OutPromptVec = std::vector<char>(OutPrompt.begin(), OutPrompt.end());

        std::vector<CTransportationPlanner::TNodeID> LastShortestPath;
        std::vector<CTransportationPlanner::TTripStep> LastFastestPath;
        bool LastWasShortest = false;
        bool LastWasFastest = false;
        double LastFastestTime = 0.0;
        double LastShortestDistance = 0.0;
        CTransportationPlanner::TNodeID LastFastestSrc = 0;
        CTransportationPlanner::TNodeID LastFastestDest = 0;
        CTransportationPlanner::TNodeID LastShortestSrc = 0;
        CTransportationPlanner::TNodeID LastShortestDest = 0;

        DOutSink->Write(OutPromptVec);

        std::vector<char> Buffer;
        char c;
        while(DCmdSrc->Get(c)){
            if (c == '\n') {
                std::string Line(Buffer.begin(), Buffer.end());
                Buffer.clear();

                auto Tokens = StringUtils::Split(Line, " \t\r");
                if (!Tokens.empty() && !Tokens[0].empty()) {
                    auto Cmd = Tokens[0];

                    if (Cmd == "help") {
                        PrintHelp();
                    } else if (Cmd == "exit") {
                        break;
                    } else if (Cmd == "count") {
                        std::string Out = std::to_string(DPlanner->NodeCount()) + " nodes\n";
                        auto OutVec = std::vector<char>(Out.begin(), Out.end());
                        DOutSink->Write(OutVec);
                    } else if (Cmd == "node") {
                        if (Tokens.size() == 2) {
                            try {
                                size_t index = std::stoull(Tokens[1]);
                                if (index < DPlanner->NodeCount()) {
                                    auto Node = DPlanner->SortedNodeByIndex(index);
                                    if (Node) {
                                        auto Loc = Node->Location();
                                        int lat_d = int(Loc.DLatitude);
                                        int lat_m = int((Loc.DLatitude - lat_d) * 60.0);
                                        int lat_s = int(std::round(((Loc.DLatitude - lat_d) * 60.0 - lat_m) * 60.0));
                                        std::string lat_dir = "N"; 
                                        int lon_d = int(std::abs(Loc.DLongitude));
                                        int lon_m = int((std::abs(Loc.DLongitude) - lon_d) * 60.0);
                                        int lon_s = int(std::round(((std::abs(Loc.DLongitude) - lon_d) * 60.0 - lon_m) * 60.0));
                                        std::string lon_dir = "W"; 

                                        std::string Out = "Node " + std::to_string(index) + ": id = " + std::to_string(Node->ID()) + 
                                                          " is at " + std::to_string(lat_d) + "d " + std::to_string(lat_m) + "' " + std::to_string(lat_s) + "\" " + lat_dir + 
                                                          ", " + std::to_string(lon_d) + "d " + std::to_string(lon_m) + "' " + std::to_string(lon_s) + "\" " + lon_dir + "\n";
                                        auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                        DOutSink->Write(OutVec);
                                    }
                                } else {
                                    std::string Out = "Invalid node parameter, see help.\n";
                                    auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                    DErrSink->Write(OutVec);
                                }
                            } catch (...) {
                                std::string Out = "Invalid node parameter, see help.\n";
                                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                DErrSink->Write(OutVec);
                            }
                        } else {
                            std::string Out = "Invalid node command, see help.\n";
                            auto OutVec = std::vector<char>(Out.begin(), Out.end());
                            DErrSink->Write(OutVec);
                        }
                    } else if (Cmd == "shortest") {
                        if (Tokens.size() == 3) {
                            try {
                                auto src = std::stoull(Tokens[1]);
                                auto dest = std::stoull(Tokens[2]);
                                LastShortestPath.clear();
                                LastShortestDistance = DPlanner->FindShortestPath(src, dest, LastShortestPath);
                                LastWasShortest = true;
                                LastWasFastest = false;
                                LastShortestSrc = src;
                                LastShortestDest = dest;

                                std::stringstream ss;
                                ss << "Shortest path is ";
                                ss << LastShortestDistance;
                                ss << " mi.\n";
                                std::string Out = ss.str();
                                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                DOutSink->Write(OutVec);
                            } catch (...) {
                                std::string Out = "Invalid shortest parameter, see help.\n";
                                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                DErrSink->Write(OutVec);
                            }
                        } else {
                            std::string Out = "Invalid shortest command, see help.\n";
                            auto OutVec = std::vector<char>(Out.begin(), Out.end());
                            DErrSink->Write(OutVec);
                        }
                    } else if (Cmd == "fastest") {
                        if (Tokens.size() == 3) {
                            try {
                                auto src = std::stoull(Tokens[1]);
                                auto dest = std::stoull(Tokens[2]);
                                LastFastestPath.clear();
                                LastFastestTime = DPlanner->FindFastestPath(src, dest, LastFastestPath);
                                LastWasFastest = true;
                                LastWasShortest = false;
                                LastFastestSrc = src;
                                LastFastestDest = dest;
                                
                                int hours = int(LastFastestTime);
                                int minutes = int((LastFastestTime - hours) * 60.0);
                                int seconds = int(std::round(((LastFastestTime - hours) * 60.0 - minutes) * 60.0));
                                if (seconds == 60) {
                                    minutes++;
                                    seconds = 0;
                                }
                                if (minutes == 60) {
                                    hours++;
                                    minutes = 0;
                                }
                                
                                std::string Out = "Fastest path takes";
                                if (hours > 0) Out += " " + std::to_string(hours) + " hr";
                                if (minutes > 0 || hours == 0) Out += " " + std::to_string(minutes) + " min";
                                if (seconds > 0) Out += " " + std::to_string(seconds) + " sec";
                                Out += ".\n";
                                
                                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                DOutSink->Write(OutVec);
                            } catch (...) {
                                std::string Out = "Invalid fastest parameter, see help.\n";
                                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                DErrSink->Write(OutVec);
                            }
                        } else {
                            std::string Out = "Invalid fastest command, see help.\n";
                            auto OutVec = std::vector<char>(Out.begin(), Out.end());
                            DErrSink->Write(OutVec);
                        }
                    } else if (Cmd == "print") {
                        if (LastWasFastest && !LastFastestPath.empty()) {
                            std::vector<std::string> desc;
                            if (DPlanner->GetPathDescription(LastFastestPath, desc)) {
                                for(auto line : desc){
                                    std::string Out = line + "\n";
                                    auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                    DOutSink->Write(OutVec);
                                }
                            }
                        } else if (LastWasShortest && !LastShortestPath.empty()) {
                            std::vector<CTransportationPlanner::TTripStep> steps;
                            for (auto nodeID : LastShortestPath) {
                                steps.push_back({CTransportationPlanner::ETransportationMode::Walk, nodeID});
                            }
                            std::vector<std::string> desc;
                            if (DPlanner->GetPathDescription(steps, desc)) {
                                for(auto line : desc){
                                    std::string Out = line + "\n";
                                    auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                    DOutSink->Write(OutVec);
                                }
                            }
                        } else {
                            std::string Out = "No valid path to print, see help.\n";
                            auto OutVec = std::vector<char>(Out.begin(), Out.end());
                            DErrSink->Write(OutVec);
                        }
                    } else if (Cmd == "save") {
                        if (LastWasFastest && !LastFastestPath.empty()) {
                            std::ostringstream ss;
                            ss << LastFastestTime << "hr.csv";
                            std::string timeStr = ss.str();
                            if (timeStr.find('.') != std::string::npos) {
                                while(timeStr.back() == '0' && timeStr[timeStr.length()-2] != '.') {
                                    timeStr.pop_back();
                                }
                                if (timeStr.find('.') + 7 > timeStr.size()) {
                                    auto idx = timeStr.find("hr.csv");
                                    std::string precision = timeStr.substr(0, idx);
                                    if(precision.find('.') != std::string::npos) {
                                        while(precision.length() - precision.find('.') - 1 < 6) {
                                            precision += "0";
                                        }
                                        timeStr = precision + "hr.csv";
                                    }
                                }
                            }
                            std::ostringstream fs;
                            fs << LastFastestSrc << "_" << LastFastestDest << "_" << std::fixed << std::setprecision(6) << LastFastestTime << "hr.csv";
                            
                            std::string Filename = fs.str();
                            auto Sink = DResults->CreateSink(Filename);
                            if (Sink) {
                                std::string Header = "mode,node_id\n";
                                auto HeaderVec = std::vector<char>(Header.begin(), Header.end());
                                Sink->Write(HeaderVec);
                                for(size_t i = 0; i < LastFastestPath.size(); ++i){
                                    const auto& step = LastFastestPath[i];
                                    std::string ModeStr = "Walk";
                                    if (step.first == CTransportationPlanner::ETransportationMode::Bike) ModeStr = "Bike";
                                    else if (step.first == CTransportationPlanner::ETransportationMode::Bus) ModeStr = "Bus";
                                    std::string LineOut = ModeStr + "," + std::to_string(step.second);
                                    if (i + 1 < LastFastestPath.size()) LineOut += "\n";
                                    auto LineVec = std::vector<char>(LineOut.begin(), LineOut.end());
                                    Sink->Write(LineVec);
                                }
                                std::string Out = "Path saved to <results>/" + Filename + "\n";
                                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                DOutSink->Write(OutVec);
                            }
                        } else if (LastWasShortest && !LastShortestPath.empty()) {
                            std::ostringstream fs;
                            fs << LastShortestSrc << "_" << LastShortestDest << "_" << std::fixed << std::setprecision(6) << LastShortestDistance << "mi.csv";
                            std::string Filename = fs.str();
                            auto Sink = DResults->CreateSink(Filename);
                            if (Sink) {
                                std::string Header = "mode,node_id\n";
                                auto HeaderVec = std::vector<char>(Header.begin(), Header.end());
                                Sink->Write(HeaderVec);
                                for(size_t i = 0; i < LastShortestPath.size(); ++i){
                                    std::string LineOut = "Walk," + std::to_string(LastShortestPath[i]);
                                    if (i + 1 < LastShortestPath.size()) LineOut += "\n";
                                    auto LineVec = std::vector<char>(LineOut.begin(), LineOut.end());
                                    Sink->Write(LineVec);
                                }
                                std::string Out = "Path saved to <results>/" + Filename + "\n";
                                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                                DOutSink->Write(OutVec);
                            }
                        } else {
                            std::string Out = "No valid path to save, see help.\n";
                            auto OutVec = std::vector<char>(Out.begin(), Out.end());
                            DErrSink->Write(OutVec);
                        }
                    } else {
                        std::string Out = "Unknown command \"" + Cmd + "\" type help for help.\n";
                        auto OutVec = std::vector<char>(Out.begin(), Out.end());
                        DErrSink->Write(OutVec);
                    }
                }
                DOutSink->Write(OutPromptVec);
            } else {
                Buffer.push_back(c);
            }
        }
        
        if (!Buffer.empty()) {
            std::string Line(Buffer.begin(), Buffer.end());
            auto Tokens = StringUtils::Split(Line, " \t\r");
            if (!Tokens.empty() && !Tokens[0].empty()) {
                auto Cmd = Tokens[0];
                if (Cmd == "exit") return true;
                
                std::string Out = "Unknown command \"" + Cmd + "\" type help for help.\n";
                auto OutVec = std::vector<char>(Out.begin(), Out.end());
                DErrSink->Write(OutVec);
            }
        }

        return true;
    }

};

CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner){
    DImplementation = std::make_unique<SImplementation>(cmdsrc, outsink, errsink, results, planner);
}

CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine(){

}

bool CTransportationPlannerCommandLine::ProcessCommands(){
    return DImplementation->ProcessCommands();
}
