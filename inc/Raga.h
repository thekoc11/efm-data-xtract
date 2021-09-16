//
// Created by abhisheknandekar on 30/12/20.
//

#ifndef PROJ_RAGA_H
#define PROJ_RAGA_H
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <unordered_set>
namespace efm {



#define MIN(a, b, c) \
    ({  __typeof__(a) _a = a; \
        __typeof__(b) _b = b; \
        __typeof__(c) _c = c; \
        __typeof__(c) tmp1, tmp2; \
        tmp1  = _a > _b ? _b : _a; \
        tmp2 = _a > _c ? _c : _a; \
        tmp1 > tmp2 ? tmp2 : tmp1; })

    template <typename T>
    struct DataItem { // a data item object that represents an element in a time-series

        DataItem() {
            value = 0;
        }

        T value;
        uint index{};
        double timestamp{};

        DataItem& operator+=(const T& rhs){
            this->value = this->value + rhs;
            return *this;
        }
        DataItem& operator-=(const T& rhs){
            this->value = this->value - rhs;
            return *this;
        }
    };


    struct Talam{
        Talam() {
            countsPerBeat = 4;
        }

        explicit Talam(uint n, std::string& na) {
            countsPerBeat = n;
            name = na;
        }

        uint countsPerBeat{};
        std::string name{};
        std::vector<std::vector<float>> durationsAndMeasures{};

    };
    template <typename T>
    bool CompareDataValuesAscending (DataItem<T>& a, DataItem<T>& b)
    {
        bool retval;
        retval = (a.value < b.value);
        return retval;
    }


    template <typename T>
    bool CompareDataValuesDescending (DataItem<T>& a, DataItem<T>& b);

    template <typename T>
    bool CompareDataIndexAscending (DataItem<T>& a, DataItem<T>& b)
    {
        return (a.index < b.index);
    }

    template <typename T>
    std::vector<int64_t> GetIndicesFromDataItems(std::vector<DataItem<T>>& vec)
    {
        std::vector<int64_t> r{};
        for(const DataItem<T> it: vec)
        {
            r.push_back(it.index);
        }
        return r;
    }


    uint GetIndexEditDistanceFromRaga(std::vector<int64_t>& v, std::vector<int64_t>& r_scale);

// this enum was originally made just to facilitate code readability. However, it serves no practical purpose whatsoever
    enum RagaNames {
        Shankarabharanam,
        Kalyani,
        Hanumatodi,
        Thodi = Hanumatodi,
        Kharaharapriya,
        Mayamalavahowla
    };


    class Raga {
    public:
        Raga() =default;
        Raga (std::string rid, std::string name, int64_t ind, std::vector<int> scale, std::string altName="None") : ragaId(std::move(rid)),
        RagaName(std::move(name)), index(ind), Scale(std::move(scale)), RagaAltName(std::move(altName)){}
        // The following should be private, however, I was too lazy to implement getters and setters for them. Hopefully does not act up later.
        std::string ragaId{};
        std::string RagaName{};
        std::string RagaAltName{};
        int64_t index{};
        std::vector<int> Scale{};

        static  std::map<std::string, std::string> InitializeVariables( const char* filename);
        static  std::vector<Talam> InitialiseTalamList(const char* filename);
        inline static std::vector<Talam> AllTalam = InitialiseTalamList("../utils/talam_list.txt");
        inline static std::map<std::string, std::string> RagaIdToRagaMapGlobal = InitializeVariables("/media/storage/RagaDataset/Carnatic/_info_/ragaId_to_ragaName_mapping.txt");

        std::vector<int64_t> GetExtededScale();

        Raga& operator=(const Raga& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            RagaName = other.RagaName;
            ragaId = other.ragaId;
            RagaAltName = other.RagaAltName;
            index = other.index;
            Scale = other.Scale;

            return *this;
        }
    };

    class Song : public Raga 
    {

        void ModifyMelodyWithF0(int64_t f); // modifies the fundamental frequency of the melody. <Look at the implementation to understand in detail>
        uint f0{};
        Talam beatData{};
        std::vector<int> notes{};
        std::vector<std::vector<int>> measurewiseNotes{};
        std::string Taal{};
        std::vector<std::string> taalVariations{};

    public:
        Song() = default;
        Song(const Song& s1);
        ~Song();
        std::string Artist{};
        std::string SongName{};
        std::string PathToFile{};
        std::string PathToData{};


        std::vector<efm::DataItem<double>> sum_MidiData;
        std::vector<std::vector<efm::DataItem<int64_t>>> MelodyData;

        void SetF0(uint f) { f0 = f; }
        void SetCountsPerBeat(int c) { beatData.countsPerBeat = c; }

        void PopulateMidiData();
        void PopulateMelodyData();

        void WriteImage(); // Writes an RGB image based on the 3d note events in the song. 
        void WriteEncodedSequenceToFile();  // Write the raw encoded audio to a text file.
        void WriteParsedNotationsToFile(const std::string& destinationPath);
        void ReadNotationsFromFile(const std::string& filename);
    };

    std::vector<Raga> GetAllRagas(const char* pathToFile);
    void InitializeKnownRagas();



    void ReadDataFiles(Song& s);


    DataItem<double> GetFundamentalFrequency(std::vector<DataItem<double>>& v, Raga& r);


    constexpr const uint ClampTo8Bit( const uint& v ) // v is some integer
    {
        return (v < 0) ? 0 : (UINT8_MAX < v) ? UINT8_MAX : v;
    }

    constexpr const uint TransformTo8Bit(const int64_t& v, const int64_t low, const int64_t high)
    {
        return ClampTo8Bit((const uint)((double)(v - low) / (high - low)  * UINT8_MAX));
    }

    void RemoveCharsFromString(std::string& str, char * charsToRemove);

};
#endif //PROJ_RAGA_H
