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

vector<efm::Song> GetAudioFiles(const char *path, bool small, const std::string& raga)
{
    const fs::path dirPath = path;

    vector<efm::Song> retVec{};
    if(fs::exists(dirPath) && fs::is_directory(dirPath))
    {
        efm::Song s;

        if(!raga.empty())
        {
            fs::path raga_path = path / fs::path(raga);
            for (auto i = fs::recursive_directory_iterator(raga_path); i != fs::recursive_directory_iterator(); ++i)
            {
                if (i.depth() == 0)
                {

                    s.ragaId = i->path().parent_path().stem();
                    s.Artist = i->path().stem();
                    if(efm::Raga::RagaIdToRagaMapGlobal.find(s.ragaId) == efm::Raga::RagaIdToRagaMapGlobal.end())
                        efm::Raga::RagaIdToRagaMapGlobal[s.ragaId] = s.ragaId;

                    s.RagaName = efm::Raga::RagaIdToRagaMapGlobal[s.ragaId];

                }
//                if ((s.ragaId == "a2f9f182-0ceb-4531-b286-b840b47a54b8") || (s.ragaId == "Celtic") || (s.ragaId == "Nottusvarams"))
//                    s.Scale = {0, 2, 4, 5, 7, 9, 11};
//                else
//                    s.Scale = {0, 2, 3, 5, 7, 8, 9, 10};



                if ((i->path().extension() == ".opus") || (i->path().extension() == ".mp3") || (i->path().extension() == ".m4a") || (i->path().extension() == ".wav"))
                {
                    s.PathToFile = i->path();
                    s.SongName = i->path().stem();

                    if (retVec.empty() || retVec.back().SongName != s.SongName)
                    {
                        retVec.push_back(s);

                    }

                }

            }
        } else if (!small)
        {
            for (auto i = fs::recursive_directory_iterator(dirPath); i != fs::recursive_directory_iterator(); ++i) {
                if (i->is_directory())
                    cout << string(i.depth() * 3, ' ') << "Current directory: " << i->path().stem().c_str()
                         << " Depth: " << i.depth() << endl;

                if (i.depth() == 0) {
                    s.ragaId = i->path().stem();
                    if(efm::Raga::RagaIdToRagaMapGlobal.find(s.ragaId) == efm::Raga::RagaIdToRagaMapGlobal.end())
                        efm::Raga::RagaIdToRagaMapGlobal[s.ragaId] = s.ragaId;

                    s.RagaName = efm::Raga::RagaIdToRagaMapGlobal[s.ragaId];

                }
                if (i.depth() == 1)
                    s.Artist = i->path().stem();



                if (i->path().extension() == ".opus" || i->path().extension() == ".m4a" ||
                    i->path().extension() == ".mp3" || i->path().extension() == ".wav") {
                    s.PathToFile = i->path();
                    s.SongName = i->path().stem();
                    retVec.push_back(s);
                }


            }
        }
        else
        {
            vector<fs::path> ragas = { path / fs::path("a2f9f182-0ceb-4531-b286-b840b47a54b8"),
                                       path / fs::path("123b09bd-9901-4e64-a65a-10b02c9e0597"),
                                       path / fs::path("Celtic"),
                                       path / fs::path("Nottusvarams")};

            for (const auto& p : ragas)
            {
                for (auto i = fs::recursive_directory_iterator(p); i != fs::recursive_directory_iterator(); ++i)
                {
                    if (i.depth() == 0)
                    {

                        s.ragaId = i->path().parent_path().stem();
                        s.Artist = i->path().stem();
                        if(efm::Raga::RagaIdToRagaMapGlobal.find(s.ragaId) == efm::Raga::RagaIdToRagaMapGlobal.end())
                            efm::Raga::RagaIdToRagaMapGlobal[s.ragaId] = s.ragaId;

                        s.RagaName = efm::Raga::RagaIdToRagaMapGlobal[s.ragaId];
                    }
//                    if (i.depth() == 1)
//                        s.Artist = i->path().stem();

                    if ((s.ragaId == "a2f9f182-0ceb-4531-b286-b840b47a54b8") || (s.ragaId == "Celtic") || (s.ragaId == "Nottusvarams"))
                        s.Scale = {0, 2, 4, 5, 7, 9, 11};
                    else
                        s.Scale = {0, 2, 3, 5, 7, 8, 9, 10};

                    if ((i->path().extension() == ".opus") || (i->path().extension() == ".mp3") || (i->path().extension() == ".m4a") || (i->path().extension() == ".wav"))
                    {
                        s.PathToFile = i->path();
                        s.SongName = i->path().stem();
                        cout << "Raga: " << s.RagaName << " Artist: " << s.Artist << "\n";
                        if (retVec.empty() || retVec.back().SongName != s.SongName)
                        {
                            retVec.push_back(s);

                        }
                    }
                }
            }
        }
//        string system_cmd = "tree ";
//        system_cmd = system_cmd.append(dirPath.parent_path().c_str());
//        std::system(system_cmd.c_str());
    }
    return retVec;
}


vector<efm::Song> GetSongInfos(const char *path, bool small, const std::string& raga) {
    const fs::path dirPath = path;
    vector<efm::Song> retVec{};
    if(fs::exists(dirPath) && fs::is_directory(dirPath))
    {
        efm::Song s;

        if(!raga.empty())
        {
            fs::path raga_path = path / fs::path(raga);
            for (auto i = fs::recursive_directory_iterator(raga_path); i != fs::recursive_directory_iterator(); ++i)
            {
                if (i.depth() == 0)
                {

                    s.ragaId = i->path().parent_path().stem();
                    s.Artist = i->path().stem();
                    if(efm::Raga::RagaIdToRagaMapGlobal.find(s.ragaId) == efm::Raga::RagaIdToRagaMapGlobal.end())
                        efm::Raga::RagaIdToRagaMapGlobal[s.ragaId] = s.ragaId;

                    s.RagaName = efm::Raga::RagaIdToRagaMapGlobal[s.ragaId];

                }
//                if ((s.ragaId == "a2f9f182-0ceb-4531-b286-b840b47a54b8") || (s.ragaId == "Celtic") || (s.ragaId == "Nottusvarams"))
//                    s.Scale = {0, 2, 4, 5, 7, 9, 11};
//                else
//                    s.Scale = {0, 2, 3, 5, 7, 8, 9, 10};



                if (i->path().extension() == ".txt")
                {
                    fs::path p_path = i->path().parent_path();
                    s.PathToData = p_path;
                    s.SongName = p_path.stem();
//                        std::cout << "Pushing " << s.SongName << " to the dataset. found at depth: " << i.depth() << '\n';

                    if (retVec.empty() || retVec.back().SongName != s.SongName)
                    {
//                            std::cout << "Pushing " << s.SongName << " to the dataset. found at depth: " << i.depth() << '\n';
                        retVec.push_back(s);

                    }

                }

            }
        }

        else if(!small)
        {
            for (auto i = fs::recursive_directory_iterator(dirPath); i != fs::recursive_directory_iterator(); ++i) {
                if (i.depth() == 0)
                    s.ragaId = i->path().stem();

                if (i.depth() == 1)
                    s.Artist = i->path().stem();


                if (i->path().extension() == ".txt") {
                    fs::path p_path = i->path().parent_path();
                    s.PathToData = p_path;
                    s.SongName = p_path.stem();
                    if (retVec.empty() || retVec.back().SongName != s.SongName)
                        retVec.push_back(s);

                }


//            if(i.depth() == 2)
//            {
//                s.PathToData = i->path();
//                s.SongName = i->path().stem();
//                retVec.push_back(s);
//            }

            }
        }
        else
        {
            vector<fs::path> ragas = { path / fs::path("a2f9f182-0ceb-4531-b286-b840b47a54b8"),
                                       path / fs::path("123b09bd-9901-4e64-a65a-10b02c9e0597"),
                                       path / fs::path("Celtic"),
                                       path / fs::path("Nottusvarams")};

            for (const auto& p : ragas)
            {
                for (auto i = fs::recursive_directory_iterator(p); i != fs::recursive_directory_iterator(); ++i)
                {
                    if (i.depth() == 0)
                    {

                        s.ragaId = i->path().parent_path().stem();
                        s.Artist = i->path().stem();
                        if(efm::Raga::RagaIdToRagaMapGlobal.find(s.ragaId) == efm::Raga::RagaIdToRagaMapGlobal.end())
                            efm::Raga::RagaIdToRagaMapGlobal[s.ragaId] = s.ragaId;

                        s.RagaName = efm::Raga::RagaIdToRagaMapGlobal[s.ragaId];

                    }
//                    if (i.depth() == 1)
//                        s.Artist = i->path().stem();

                    if ((s.ragaId == "a2f9f182-0ceb-4531-b286-b840b47a54b8") || (s.ragaId == "Celtic") || (s.ragaId == "Nottusvarams"))
                        s.Scale = {0, 2, 4, 5, 7, 9, 11};
                    else
                        s.Scale = {0, 2, 3, 5, 7, 8, 9, 10};



                    if (i->path().extension() == ".txt")
                    {
                        fs::path p_path = i->path().parent_path();
                        s.PathToData = p_path;
                        s.SongName = p_path.stem();
                        std::cout << "Pushing " << s.SongName << " " << s.RagaName << " to the dataset. found at depth: " << i.depth() << '\n';

                        if (retVec.empty() || retVec.back().SongName != s.SongName)
                        {
//                            std::cout << "Pushing " << s.SongName << " to the dataset. found at depth: " << i.depth() << '\n';
                            retVec.push_back(s);

                        }

                    }


                }
            }
        }
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
