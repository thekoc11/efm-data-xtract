//
// Created by abhisheknandekar on 08/01/21.
//

#ifndef PROJ_STRINGOPS_H
#define PROJ_STRINGOPS_H
#include <string>
#include <regex>
#include <utility>
#include <vector>

namespace efm{

    typedef struct NoteName{
        NoteName() = default;
        NoteName(std::string val, std::string acc, std::string oct)
        {
            value = std::move(val);
            accent = std::move(acc); // flat (represented by ) or sharp (represented by )
            octave = std::move(oct);
        }
        std::string value{};
        std::string accent{};
        std::string octave{};
    }NoteName;


  struct Note{
      Note(int64_t ind,  uint F0);
      Note() { initName(0); }

      void initName(const int64_t& ind);
      std::string GetName();

      int64_t RelativeIndex{};
      int64_t RelativeOctave{};
      NoteName Name{};
      uint F0{};
  };

  int GetNumVowelsInLine(const std::string& line); // Helps in the detection of the svarams in textual notations
  int GetRestCharactersInArray(const std::vector<int>& line);
};
#endif //PROJ_STRINGOPS_H
