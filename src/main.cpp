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
int replica = 10;

using namespace std;
using json = nlohmann::json;

// Function declaration
mJob Data_Resolver(string);

string path = "/home/airobots/MyScheduler/data/myData/mData";


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
    ofstream best_fitness_file;
    ofstream best_time_file;
    ofstream avg_fitness_file;
    ofstream avg_time_file;
    ofstream sd_fitness_file;
    ofstream sd_time_file;
    ofstream change_fitness_file;

    double *avg_fitness_accum = new double[Max_iter];



    for (int i = 183; i < 1361; i++) {
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
        vector<uint16_t> fit;
        vector<double> time;
        for (int j = 0; j < replica; j++) {
            
            if (algorithm.compare("SA") == 0){
                CSA->RunAlgorithm();
                fit.push_back(CSA->GetSolution());
                time.push_back(CSA->GetCalculateTime());
                
            }
            else if (algorithm.compare("GA") == 0){
                CGA->RunAlgorithm();
                fit.push_back(CGA->GetSolution());
                time.push_back(CGA->GetCalculateTime());
            }
            else if (algorithm.compare("GP") == 0){
                CGP->RunAlgorithm();
                fit.push_back(CGP->GetSolution());
                time.push_back(CGP->GetCalculateTime());
                
            }
            
        }

        // calculate avg
        float avg_fit = 0;
        double avg_time = 0;
        for (int j = 0; j < replica; j++) {
            avg_fit += fit[j];
            avg_time += time[j];
        }
        avg_fit = ((float)avg_fit/replica);
        avg_time = avg_time / replica;

        solution.push_back(avg_fit);
        calculate_time.push_back(avg_time);

        // calculate standard deviation of fitness
        double sd_fit = 0;
        for (int j = 0; j < replica; j++) {
            sd_fit += pow((fit[j] - avg_fit), 2);
        }
        sd_fit = sqrt(sd_fit / replica);

        // print best solution
        uint16_t best_solution = UINT16_MAX;
        double bestTime = 0;
        for (int j = 0; j < replica; j++) {
            if (fit[j] < best_solution) {
                best_solution = fit[j];
                bestTime = time[j];
            }
        }

        // print best time
        cout << "avg time: " << avg_time << endl;
        cout << "avg Fitness " << avg_fit << endl;
        
        best_fitness_file.open("/home/airobots/MyScheduler/data/results/MD_best_fit_M" + ALGORITHM + ".csv", ios::app);
        best_time_file.open("/home/airobots/MyScheduler/data/results/MD_best_time_M" + ALGORITHM + ".csv", ios::app);
        avg_fitness_file.open("/home/airobots/MyScheduler/data/results/MD_avg_fit_M" + ALGORITHM + ".csv", ios::app);
        avg_time_file.open("/home/airobots/MyScheduler/data/results/MD_avg_time_M" + ALGORITHM + ".csv", ios::app);
        sd_fitness_file.open("/home/airobots/MyScheduler/data/results/MD_sd_fit_M" + ALGORITHM + ".csv", ios::app);
        sd_time_file.open("/home/airobots/MyScheduler/data/results/MD_sd_time_M" + ALGORITHM + ".csv", ios::app);
        best_fitness_file << best_solution << "," << endl;
        best_time_file << bestTime << "," << endl;
        avg_fitness_file << avg_fit << "," << endl;
        avg_time_file << avg_time << "," << endl;
        sd_fitness_file << sd_fit << "," << endl;
        sd_time_file << avg_time << "," << endl;

        best_fitness_file.close();
        best_time_file.close();
        avg_fitness_file.close();
        avg_time_file.close();
        sd_fitness_file.close();
        sd_time_file.close();

        if (algorithm.compare("GP") == 0) {
            for (int j = 0; j < Max_iter; j++) {
                avg_fitness_accum[j] += (CGP->GetBestFitness(j));
            }
        }

        
        CPU_time += avg_time;
    }


    change_fitness_file.open("/home/airobots/MyScheduler/data/results/MD_fit_change_M" + ALGORITHM + ".csv", ios::app);
    for (int i = 0; i < Max_iter; i++) {
        change_fitness_file << avg_fitness_accum[i] / 1360 << "," << endl;
    }
    change_fitness_file.close();

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
