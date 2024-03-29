//
// Created by abhisheknandekar on 08/01/21.
// Gives you the ability to "visualize" the notations paarsed from audio files  
// 
// 
// 
// 
#include "stringops.h"

static const std::string NoteNames[8] = {"Sa", "Ri", "Ga", "Ma", "Pa", "Dha", "Ni", "|"};
static const char ACCENTS[2] = {'#', '!'};
static const char OCTAVES[2] = {'.', '^'};

efm::Note::Note(int64_t ind, uint f0)
{
    initName(ind);
    F0 = f0;

}

static int64_t returnPositiveNote(int64_t idx)
{
    if(idx >= 0)
        return idx;
    else
        return returnPositiveNote(idx + 12);
}


void efm::Note::initName(const int64_t& ind)
{
    RelativeIndex = ind;

    int64_t n, series, note_idx;
    int64_t cor_ind = returnPositiveNote(ind);
    n = cor_ind % 12;
    series = (ind < 0) ? (ind / 12) - 1 : ind / 12;

    RelativeOctave = series;

    if(n < 7)
        note_idx = (n % 2 == 0) ? n / 2 : ( n + 1 ) / 2;
    else
        note_idx = (n % 2 == 0) ? n / 2 + 1 : (n + 1) / 2;

    if(ind > 999) {
        note_idx = 7;
        series = 0;
        n = 0;
    }

    Name.value = NoteNames[note_idx];
    Name.octave = (series < 0) ? std::string(-series, OCTAVES[0]) : std::string(series, OCTAVES[1]);

    if(n==1 || n == 3 || n == 8 || n == 10)
        Name.accent = ACCENTS[1];
    else if (n == 6)
        Name.accent = ACCENTS[0];


}


std::string efm::Note::GetName() {

    initName(RelativeIndex);
    return Name.value + Name.accent + Name.octave;
}

int efm::GetNumVowelsInLine(const std::string& line)
{
    char Vowels[10] = {'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
    std::istringstream iss(line);
    std::string ts{};
    int nVowelsInLine = 0;
    while (iss >> ts)
    {
        for (const auto& c: Vowels)
        {
            auto found = ts.find(c);
            if(found != std::string::npos)
            {
                ++nVowelsInLine;
            }
        }
    }
    return nVowelsInLine;
}
int efm::GetRestCharactersInArray(const std::vector<int>& line)
{
    int num_rests = 0;
    for (auto& item: line)
    {
        if (item == 99999)
            num_rests++;
    }
    return num_rests;
}
