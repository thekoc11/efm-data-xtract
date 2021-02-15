//
// Created by theko on 03/12/20.
//

#ifndef PROJ_FILEOPS_HPP
#define PROJ_FILEOPS_HPP

//#inc <dirent.h>
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <system_error>
#include <sys/stat.h>
#include "./Raga.h"

namespace fs = std::filesystem;
using namespace std;



vector<fs::path> GetFilesInPath(const char* path);
void PopulateAudioFiles(const char* dir, const vector<string>& extensions);
vector<efm::Song> GetAudioFiles(const char* path, bool small, const std::string& raga);
vector<efm::Song> GetSongInfos(const char* path, bool small, const std::string& raga);
void CreateDirectory(const char* dir);
vector<byte> LoadFile(const char* filePath);

#endif //PROJ_FILEOPS_HPP
