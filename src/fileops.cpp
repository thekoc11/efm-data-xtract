//
// Created by theko on 03/12/20.
//

#include "../inc/fileops.hpp"

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
