//
// Created by theko on 03/12/20.
//

#ifndef PROJ_FILEOPS_HPP
#define PROJ_FILEOPS_HPP

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

vector<fs::path> GetFilesInPath(const char* path);

#endif //PROJ_FILEOPS_HPP
