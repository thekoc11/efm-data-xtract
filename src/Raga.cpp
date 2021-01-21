//
// Created by abhisheknandekar on 30/12/20.
//

#include <Raga.h>
#include "../inc/fileops.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>
#include "stringops.h"


using namespace efm;

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

void efm::Song::PopulateMidiData() {


}

void efm::Song::PopulateMelodyData() {

}


void Song::WriteToFile() {

    fs::path path = PathToData;

    if (SongName.empty())
    {
        SongName = path.stem();
    }
    ModifyMelodyWithF0(f0);
    const int64_t num_elements = MelodyData.size();
    bool writable = false;

    std::vector<std::vector<Note>> MelodyNotes{};
    uint numRests = 0;
    std::string writableChars{};
    std::ofstream file(path /= (SongName + ".txt"));


    for (auto& m : MelodyData)
    {
        std::vector<Note> row(4); std::string rowNames[4];
        row[0].initName(m[0].value);
        row[1].initName(m[1].value);
        row[2].initName(m[2].value);
        row[3].initName(m[3].value);
        rowNames[0] = row[0].GetName();
        rowNames[1] = row[1].GetName();
        rowNames[2] = row[2].GetName();
        rowNames[3] = row[3].GetName();

        if(m[1].value == 9999 && m[2].value == 9999)
        {
            ++numRests;
            writableChars = "\t";
        }
        else{
            numRests = 0;
            std::string wrtVal0 = (m[0].value == 9999) ? "<unk>" : std::to_string(m[0].value + 36);
            std::string wrtVal1 = (m[1].value == 9999) ? "<unk>" : std::to_string(m[1].value + 36);
            std::string wrtVal2 = (m[2].value == 9999) ? "<unk>" : std::to_string(m[2].value + 36);
            std::string wrtVal3  = (m[3].value == 9999) ? "<unk>" : std::to_string(m[3].value + 36);
            writableChars = wrtVal0.append( " ").append(wrtVal1) + " " + wrtVal2 + " " + wrtVal3.append(" ");
        }

        if(numRests == 1 || numRests == 0)
            writable = true;
        else if (numRests == 10)
        {
            writable = true;
            writableChars = "\n";
        }
        else
            writable = false;

        if(writable)
            file << writableChars;
    }
    cout << "\nWrite to File Successful... \n\n ";
}

void Song::WriteImage()
{
    fs::path path = PathToData;

    if (SongName.empty())
    {
        SongName = path.stem();
    }
    ModifyMelodyWithF0(f0);

    const int64_t num_elements = MelodyData.size();
    int64_t new_size;
    int64_t height = sqrt(num_elements);

    DataItem<int64_t> value{};

    std::cout << "Total ELements begore  resize: " << num_elements <<std::endl;



    if(num_elements % height != 0)
    {
        new_size = num_elements + height - (num_elements % height);
        if(new_size > num_elements)
        {
            for(auto x = num_elements; x < new_size; ++x)
            {
                std::vector<DataItem<int64_t>> newVecs{};

                DataItem<int64_t> it{};
                it.value = 0;

                for(auto y = 0; y < 4; ++y)
                {
                    it.index =  y;
                    newVecs.push_back(it);
                }

                MelodyData.push_back(newVecs);
            }
        }

    }


    int64_t width = MelodyData.size() / height;
    std::cout << "Elements after resize " << MelodyData.size() << " Height " << height << " width " << width << std::endl;

    std::ofstream img(path /= "picture.ppm");
    img << "P3" << std::endl;
    img << width << " " << height << std::endl;
    std::cout << "Strating Image write \n";

    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {

            int r = TransformTo8Bit(MelodyData[y*width + x][1].value, -12, 24);
            int g = TransformTo8Bit(MelodyData[y*width + x][2].value, -12, 24);
            int b = TransformTo8Bit(MelodyData[y*width + x][3].value, -12, 24);

            img << r << " " << g << " " << b << std::endl;
        }
    }
    std::cout << "Image write Successful\n";
//    std::string systemcmd = "xdg-open " + path.string();
//    std::system(systemcmd.c_str());
}

void Song::ModifyMelodyWithF0(int64_t f)
{
    for(auto& row : MelodyData)
    {
        for(DataItem<int64_t>& ele : row)
        {
            if (ele.index >= 0 && (ele.value - f) > -36)
                ele -= f;
            else
                ele.value = 9999;
        }
    }

}

Song::Song(const Song &s1)  : Raga(s1) {
    Artist = s1.Artist;
    SongName = s1.SongName;
    PathToData = s1.PathToData;
    PathToFile = s1.PathToFile;
    f0 = s1.f0;


    ragaId = s1.ragaId;
    RagaName = s1.RagaName;

    sum_MidiData.assign(s1.sum_MidiData.begin(), s1.sum_MidiData.end());
    MelodyData.assign(s1.MelodyData.begin(), s1.MelodyData.end());


}

Song::~Song() {

    std::vector<efm::DataItem<double>>().swap(sum_MidiData);
    std::vector<std::vector<efm::DataItem<int64_t>>>().swap(MelodyData);
}

std::map<std::string, std::string> Raga::InitializeVariables(const char* filename) {


    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if(!ifs)
        throw std::runtime_error(std::string(filename) + ": " + std::strerror(errno));
    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    auto size = std::size_t (end - ifs.tellg());

    if(size == 0)
        throw std::runtime_error(std::string(filename) + ": " + std::strerror(errno));

    std::vector<unsigned char> buffer(size);

    if(!ifs.read((char*)buffer.data(), buffer.size()))
        throw std::runtime_error(std::string(filename) + ": " + std::strerror(errno));

    bool id_done = false;
    std::string ragaId{};
    std::string raga{};
    std::map<std::string, std::string> ragaIdToRagaMap{};
    for (auto i : buffer)
    {
        char i_char = (char) i;
        if(i_char == '\t')
            id_done = true;
        if(!id_done)
            ragaId += i_char;
        else if (i_char != '\t' && i_char != '\n')
            raga += i_char;

        if(i_char == '\n')
        {
            id_done = false;
            ragaIdToRagaMap[ragaId] = raga;
            ragaId = {};
            raga = {};
        }
    }

    return ragaIdToRagaMap;

}

std::vector<int64_t> efm::Raga::GetExtededScale() {

    std::vector<int64_t> retScale;
    for(auto ele: Scale)
    {
        auto e = (int64_t)ele;
        retScale.push_back(-e- 1);
        retScale.push_back(-e- 13);
        retScale.push_back(e);
        retScale.push_back(e + 12);
        retScale.push_back(e + 24);
    }
    std::sort(retScale.begin(), retScale.end());
//
//    std::cout << "Final Raga Scale:\n";
//    for(const auto ele: retScale)
//    {
//        std::cout << ele << "\t";
//    }
//    std::cout << std::endl;

    return retScale;
}


template <typename T>
bool efm::CompareDataValuesDescending(DataItem<T> &a, DataItem<T> &b)
{
    bool retVal;
    retVal = (a.value > b.value);

    return retVal;
}


void efm::ReadDataFiles(Song &s) {

    std::vector<std::vector<efm::DataItem<double>>> v;
    DataItem<double> temp{};
    temp.value = 0.0; temp.index = 0;
    std::vector<efm::DataItem<double>> sum_v(135, temp);
    std::string midi_table_path = s.PathToData + "/transformed.txt";
    std::ifstream ifs(midi_table_path, std::ios::binary);

    std::string tempstr;
    uint row_iter = 0;
    while (std::getline(ifs, tempstr))
    {
        std::istringstream iss(tempstr);
        std::vector<efm::DataItem<double>> tempv;
        double ts = 0;
        iss >> ts;
        std::string val_str;
        double val = 0;
        uint col_iter = 0;
        while (iss >> val_str)
        {
            efm::DataItem<double> tmpDataItem{};
            if(val_str != "-nan")
                val = stod(val_str);
            else
                val = 0;
            tmpDataItem.value = val;
            tmpDataItem.index = col_iter;
            tmpDataItem.timestamp = ts;
            sum_v[col_iter].value += val;
            sum_v[col_iter].index = col_iter;


            tempv.push_back(tmpDataItem);
            col_iter++;
        }
        row_iter++;
        v.push_back(tempv);
        std::vector<efm::DataItem<double>>().swap(tempv);
    }

    std::sort(sum_v.begin(), sum_v.end(), efm::CompareDataValuesDescending<double>);

    sum_v.resize(18);
    std::sort(sum_v.begin(), sum_v.end(), efm::CompareDataIndexAscending<double>);


//    s.MidiData = v;
    s.sum_MidiData.assign(sum_v.begin(), sum_v.end());


    std::string melody_path = s.PathToData + "/melody.txt";
    std::vector<std::vector<efm::DataItem<int64_t>>> Melody;
    std::ifstream mel(melody_path, std::ios::binary);
    std::vector<efm::DataItem<double>>().swap(sum_v);
    std::vector<std::vector<efm::DataItem<double>>>().swap(v);


    row_iter = 0;
    while (std::getline(mel, tempstr))
    {
        std::istringstream iss(tempstr);
        std::vector<efm::DataItem<int64_t>> tempv;
        double ts = 0;
        iss >> ts;
        int64_t val;
        uint col_iter= 0;
        while(iss >> val)
        {
            efm::DataItem<int64_t> tmpItm{};
            tmpItm.value = val;
            tmpItm.index = col_iter;
            tmpItm.timestamp = ts;
            tempv.push_back(tmpItm);
            ++col_iter;
        }
        Melody.push_back(tempv);
        row_iter++;
        std::vector<efm::DataItem<int64_t>>().swap(tempv);
    }

    s.MelodyData.assign(Melody.begin(), Melody.end());
    std::cout << "Reading Completed Sccessfully!! " << s.sum_MidiData.size() << std::endl;
    std::vector<std::vector<efm::DataItem<int64_t>>>().swap(Melody);
}

uint efm::GetIndexEditDistanceFromRaga(std::vector<int64_t> &v, std::vector<int64_t> &r_scale)
{
    const int m = v.size();
    const int n = r_scale.size();
    int dp[m + 1][n + 1];

    for(int i = 0; i <= m; i++)
    {
        for(int j = 0; j <= n; j++)
        {
            if (i==0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (v[i - 1] == r_scale[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j]
                        = 1 + MIN(dp[i][j - 1],
                                  dp[i - 1][j],
                                  dp[i-1][j-1]);
        }

    }

    return dp[m][n];
}

DataItem<double> efm::GetFundamentalFrequency(std::vector<DataItem<double>> &v, Raga &r)
{
    DataItem<double> ret{};
    double min_editDistance = 999999;
    double sum_EtoZ = 0;
    uint f0;
    std::vector<DataItem<double>> f0s;
    auto ex_r_scale = r.GetExtededScale();
    for (auto ele : v)
    {
        std::vector<DataItem<double>> v_dupl(v);
        for (auto& it: v_dupl)
            it.index -= ele.index;

        auto index_values = GetIndicesFromDataItems(v_dupl);
        uint ed = GetIndexEditDistanceFromRaga(index_values, ex_r_scale);
        double e_dist = (double)ed * (1 );/// ele.value);
        if(e_dist < min_editDistance)
        {
            f0s.clear();
            min_editDistance = e_dist;
            f0 = ele.index;
            sum_EtoZ = exp(-ele.value);
            DataItem<double> f {};
            f.value = exp(-ele.value);
            f.index = f0;
            f0s.push_back(f);
        }
        else if(e_dist == min_editDistance)
        {
            sum_EtoZ += exp(-ele.value);
            DataItem<double> f {};
            f.value = exp(-ele.value);
            f.index = ele.index;
            f0s.push_back(f);
        }

//        std::cout << "Freq: " << ele.index << " Distance: " << e_dist << std::endl;

    }
    double min_ceDistance = 99999;
    if(f0s.size() > 1)
    {
        for(auto i : f0s)
        {
            double d = i.value / sum_EtoZ;
            if(d > min_ceDistance)
            {
                min_ceDistance = d;
                f0 = i.index;
            }
        }
    }

    ret.value = min_editDistance;
    ret.index = f0;

    return ret;
}

void efm::RemoveCharsFromString(string &str, char *charsToRemove)
{
    for(uint i = 0; i < std::strlen(charsToRemove); ++i)
    {
        str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}

