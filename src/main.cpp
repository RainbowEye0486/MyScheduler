# include "projectConfig.h"
# include "nlohmann/json.hpp"
# include <stdio.h>
# include <stdlib.h>
# include <cstring>
# include <fstream>
# include <unistd.h>
# include <iostream>
# include "../include/SA.h"
# include "../include/GA.h"
# include "../include/GP.h"



string ALGORITHM = "GP";


using namespace std;
using json = nlohmann::json;

// Function declaration
mJob Data_Resolver(string);

string path = "/home/airobots/myScheduler/data/myData/mData";


int main (int argc, char *argv[])
{
    
    string algorithm = ALGORITHM;
    
    //just print version of the project
    cout << "\n==============Calvin Scheduler Simulation Start==============" << endl;
    cout << "\033[1;32m[SYSTEM]:Project Version Major: " << PROJECT_VERSION_MAJOR << "\033[0m" << endl;
    cout << "\033[1;32m[SYSTEM]:Projevt Version Minor: " << PROJECT_VERSION_MINOR << "\033[0m" << endl;
    cout << "\033[1;32m[SYSTEM]:Project Version Patch: " << PROJECT_VERSION_PATCH << "\033[0m" << endl;
    cout << "\033[1;32m[SYSTEM]: Calvin Scheduler Start\033[0m" << endl;
    // Problem in Damla Kizilay's Problem
    
    SA *CSA = SA::GetSA();
    GA *CGA = GA::GetGA();
    GP *CGP = GP::GetGP();
    
    int dataset_cnt = 1;

    vector<uint16_t> solution;
    
    vector<double> calculate_time; 

   
    int optimal_cnt = 0;
    double CPU_time = 0;
    ofstream station_file;
    ofstream timelog_file;
    ofstream avg_fitness_file;

    double *avg_fitness_accum = new double[Max_iter];



    for (int i = 1337; i < 1338; i++) {
        string data_path = path + to_string(i) + ".json";
        // sleep for 0.05 second
        usleep(50000);

        mJob job = Data_Resolver(data_path);
        cout << "[INFO]: Proceccing Data " << i << endl;
        if (algorithm.compare("SA") == 0){
            CSA->PreProcessing(job);
        }
        else if (algorithm.compare("GA") == 0){
            CGA->PreProcessing(job);
        }
        else if (algorithm.compare("GP") == 0){
            CGP->PreProcessing(job);
        }
        uint16_t best_solution = UINT16_MAX;
        double bestTime = 0;
        for (int j = 0; j < 1; j++) {
            
            if (algorithm.compare("SA") == 0){
                CSA->RunAlgorithm();
                if (CSA->GetSolution() < best_solution){
                    best_solution = CSA->GetSolution();
                    bestTime = CSA->GetCalculateTime();
                }
            }
            else if (algorithm.compare("GA") == 0){
                CGA->RunAlgorithm();
                if (CGA->GetSolution() < best_solution){
                    best_solution = CGA->GetSolution();
                    bestTime = CGA->GetCalculateTime();
                }
            }
            else if (algorithm.compare("GP") == 0){
                CGP->RunAlgorithm();
                if (CGP->GetSolution() < best_solution){
                    best_solution = CGP->GetSolution();
                    bestTime = CGP->GetCalculateTime();
                }
            }
            
        }
        solution.push_back(best_solution);
        calculate_time.push_back(bestTime);

        // print best time
        cout << "Best time: " << bestTime << endl;
        cout << "Fitness " << best_solution << endl;
        
        station_file.open("/home/airobots/myScheduler/data/results/fully_fitness_" + ALGORITHM + "-20.csv", ios::app);
        timelog_file.open("/home/airobots/myScheduler/data/results/fully_timelog_" + ALGORITHM + "-20.csv", ios::app);
        station_file << best_solution << "," << endl;
        timelog_file << bestTime << "," << endl;
        station_file.close();
        timelog_file.close();

        for (int j = 0; j < Max_iter; j++) {
            avg_fitness_accum[j] += (CGP->GetAvgFitness(j));
        }
        
        CPU_time += bestTime;
    }


    avg_fitness_file.open("/home/airobots/myScheduler/data/results/fully_avg_fitness_" + ALGORITHM + "-20.csv", ios::app);
    for (int i = 0; i < Max_iter; i++) {
        avg_fitness_file << avg_fitness_accum[i] / 1360 << "," << endl;
    }
    avg_fitness_file.close();

    cout << "\033[1;32m[SYSTEM]: Simulation over.\033[0m" << endl;
        
    return 0;
}


/*
Read json file as dataset input
*/
mJob Data_Resolver(string dataName){
    std::ifstream f(dataName);
    if (!f)
        cout << "\033[1;31m[ERROR]:File not found!\033[0m" << endl;   
    json data = json::parse(f);

    uint16_t _taskNum = (uint16_t)data["taskNum"];

    vector<mtask> _task_list;
    for(int i = 0; i < _taskNum; i++) {
        vector<uint16_t> _and_prev;
        vector<uint16_t> _and_next;
        vector<uint16_t> _or_prev;
        vector<uint16_t> _or_next;
        string exe = data["task"][i]["operator"];
        char _executor = exe[0];
        for(int j = 0; data["task"][i]["and"][j] != nullptr; j++) 
            _and_next.push_back((uint16_t)data["task"][i]["and"][j]);
        for(int j = 0; data["task"][i]["or"][j] != nullptr; j++) 
            _or_next.push_back((uint16_t)data["task"][i]["or"][j]);
        mtask tmp_task = {
            .num = (uint16_t)data["task"][i]["number"],
            .executor = _executor,
            .r_time = (uint16_t)data["task"][i]["time"][0],
            .h_time = (uint16_t)data["task"][i]["time"][1],
            .and_prev = _and_prev,
            .and_next = _and_next,
            .or_prev = _or_prev,
            .or_next = _or_next
        };
        _task_list.push_back(tmp_task);
    }

    mJob job = {
        .taskNum = _taskNum,
        .task_list = _task_list
    };
    if (&job == nullptr) 
        cout << "\033[1;31m[ERROR]:Data resolve failed!\033[0m" << endl; 
    

    // cout << "[INFO]: Data resolve over." << endl;
    return job;
}
