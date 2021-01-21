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
            accent = std::move(acc);
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
};
#endif //PROJ_STRINGOPS_H
