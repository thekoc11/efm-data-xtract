//
// Created by theko on 03/12/20.
//

#include <cstring>
#include "../inc/fileops.hpp"

static const string UP_DIR = "..";
static const string CURRENT_DIR = ".";



vector<fs::path> GetFilesInPath(const char *path) {

    const fs::path dirPath = path;
    vector<fs::path> retVec{};

    if(fs::exists(dirPath) && fs::is_directory(dirPath))
    {
        for(const auto& entry : fs::directory_iterator(dirPath))
        {
            const auto& filename = entry.path();
            if(fs::is_directory(entry.status()))
            {
//                cout << filename.filename().string().c_str() << " is a directory" << endl;
            }
            else if(fs::is_regular_file(entry.status()))
            {
//                cout << filename.filename() << endl;
                retVec.push_back(filename);
            }
            else
            {
                cout << "Format not recognised/Not a regular file [?]" << filename << endl;
            }
        }
    }

//    cout << "Total " << retVec.size() << " files found " << endl;

    return retVec;
}






void CreateDirectory(const char *dir)
{
    const fs::path dirPath = dir;
    if(!fs::is_directory(dirPath) || !fs::exists(dirPath))
    {
        if(!fs::exists(dirPath))
        {
            fs::create_directories(dirPath);
        }
        else
            cerr << "File named " << dir << " exists and is not a directory! \n";
    }

    string system_cmd = "tree ";
    system_cmd = system_cmd.append(dirPath.parent_path().c_str());
    std::system(system_cmd.c_str());

}

vector<efm::Song> GetAudioFiles(const char *path)
{
    const fs::path dirPath = path;

    vector<efm::Song> retVec{};
    if(fs::exists(dirPath) && fs::is_directory(dirPath))
    {
        efm::Song s;

        for(auto i = fs::recursive_directory_iterator(dirPath); i != fs::recursive_directory_iterator(); ++i)
        {
//            if(i->is_directory())
//                cout << string(i.depth() * 3, ' ') << "Current directory: " << i->path().stem().c_str() << " Depth: " << i.depth() << endl;

            if(i.depth() == 0)
                s.ragaId = i->path().stem();
            if(i.depth() == 1)
                s.Artist = i->path().stem();

            if(i->path().extension() == ".opus" || i->path().extension() == ".m4a" || i->path().extension() == ".mp3" || i->path().extension() == ".wav")
            {
                s.PathToFile = i->path();
                s.SongName = i->path().stem();
                retVec.push_back(s);
            }



        }

//        string system_cmd = "tree ";
//        system_cmd = system_cmd.append(dirPath.parent_path().c_str());
//        std::system(system_cmd.c_str());
    }
    return retVec;
}

vector<byte> LoadFile(const char *filePath)
{
    ifstream ifs(filePath, std::ios::binary | std::ios::ate);

    if(!ifs)
        throw std::runtime_error((std::string(filePath) + ": " + strerror(errno)));
    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    auto size = std::size_t(end - ifs.tellg());

    if(size == 0)
        return {};
    vector<byte> buffer(size);
    if(!ifs.read((char*)buffer.data(), buffer.size()))
        throw std::runtime_error((std::string(filePath) + ": " + strerror(errno)));

    return buffer;
}
