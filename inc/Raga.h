//
// Created by abhisheknandekar on 30/12/20.
//

#ifndef PROJ_RAGA_H
#define PROJ_RAGA_H
#include <string>
#include <vector>

namespace efm {

    class Raga {
    public:
        Raga()
        = default;
        std::string ragaId{};
        std::string RagaName{};
        int64_t index{};
        std::vector<uint> Scale{};

    };

    class Song : public Raga
    {
    public:
        Song() = default;
        std::string Artist{};
        std::string SongName{};
        std::string PathToFile{};
        std::string PathToData{};
    };

    std::vector<Raga> GetAllRagas(const char* pathToFile);

};
#endif //PROJ_RAGA_H
