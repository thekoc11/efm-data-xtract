//
// Created by abhisheknandekar on 30/12/20.
//

#include <Raga.h>
#include "../inc/fileops.hpp"

std::vector <efm::Raga> efm::GetAllRagas(const char *pathToFile)
{
    vector<efm::Raga> ragas{};
    vector<byte> ragaIdText = LoadFile(pathToFile);

    if(!ragaIdText.empty())
    {
        efm::Raga raga;
        uint iter = 0;
        bool id_dome = false;
        for(auto i : ragaIdText)
        {
            char i_char = (char)i;
            if ((char)i == '\t')
                id_dome = true;


            if(!id_dome)
                raga.ragaId += i_char;
            else if((char)i != '\t' && (char)i != '\n')
                raga.RagaName += i_char;

            if((char)i == '\n')
            {
                id_dome = false;
                ragas.push_back(raga);
                raga = {};

            }

        }

    }

    return ragas;
}

