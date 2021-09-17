#include <iostream>
#include <filesystem>
#include <chrono>
#include <string>
#include <map>
#include <algorithm>
#include "../inc/filter.hpp"
#include "../inc/fileops.hpp"
#include "../inc/Raga.h"
#include "taskflow/taskflow.hpp"

                            // filter params for the customized showcqt filter  
const char *filters_descr = "highpass=400, lowpass=4000, aresample=44100, aformat=sample_fmts=s16:channel_layouts=mono, volume=10.5, showcqt=s=960x540:fcount=1:no_video=1:no_data=0:tc=0.043:sono_v=a_weighting(f):storepath=/media/storage/audio_data/tu_mera_nahi"; //showcqt=tc=0.17:tlength='st(0,0.17); 384*tc / (384 / ld(0) + tc*f /(1-ld(0))) + 384*tc / (tc*f / ld(0) + 384 /(1-ld(0)))':sono_v=a_weighting(f)*16:sono_h=980"; // dynaudnorm [a]; [a] lowpass=frequency=4000 [b]; [b] aresample=4000,aformat=sample_fmts=s16:channel_layouts=mono[c]; [c]
//const char *filter_descr = "aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono"; s=192x108:
namespace fs = std::filesystem;
namespace ch = std::chrono;
//const char* RagaIdPath = "/media/storage/RagaDataset/Carnatic/_info_/ragaId_to_ragaName_mapping.txt";

#define PARALLEL 1

static void show_usage(const std::string& name)
{
    std::cerr << "Usage: " << name << " <operation(s)> SOURCES/DESTINATIONS\n "
              << "Options: \n"
              << "\t-x, --extract SOURCE \t\textract data mode. SOURCE is a directory which contains audios in SOURCE/audio and other essential conversion files in the SOURCE/_info_ subfolders\n"
              << "\t-p, --process SOURCE \t\tprocess data mode. SOURCE is usually the DESTINATION resulting from the --extract option\n"
              << "\t-d, --destination DESTINATION \t Specify the destination path for the current processes\n"
              << "\t-fd, --full_dataset \t\t Use the full dataset. Disabled by default.\n"
              << "\t-h, --help \t\t display this help message\n"
              << "\t-rf, --parse FILENAME.txt \t\t read and interpret the notations given in the contents of FILENAME. In case the destination is not specified using -d, the final output FILENAME-parsed.txt is saved in the same folder\n"
              << "\t-nb, --num_beats X \t\t IMPORTANT: USE THIS WHILE USING -rf; Number of beats in the concerned song (used for parsing)\n"
              << std::endl;
}


static int ApplyFilter(const char* filename, const char* filter_descr); // Apply the showcqt filter on filename using filter_descr as filter params
static int FilterAndExtractData(const std::string& source, const std::string& dest, bool small, const std::string& raga = "");
static int ProcessExtractedData(const std::string& source, const std::string& dest, bool small, const std::string& raga = "");
//// TODO: Define a function for finding the tonic. Make a parallelised version for finding the tonic at a massive scale
static int CreateDataset(const std::string& source, const std::string& ragaId, const std::string& dest); // Used with the CompMusic Raga dataset.
static int ReadNotations(const std::string& filename, const std::string& destination_folder, int num_beats); // Used with textual notations

int main(int argc, char** argv) {

    int ret = -9999;
    bool small = true;
// If CARNATIC_RAGAID_RAGA_PATH to the ragaId_to_ragaName_mapping.txt file in the Carnatic folder of the CompMusic Dataset, the following lines may be uncommented
//    std::map<std::string, string> idRagaMap = efm::Raga::InitializeVariables(CARNATIC_RAGAID_RAGA_PATH);
//    efm::Raga::RagaIdToRagaMapGlobal = idRagaMap;
//    cout << "Total Num Ragas in the dataset:(called from main() " << efm::Raga::RagaIdToRagaMapGlobal.size() << '\n';
    cout << "Total Num Taalams found: " << efm::Raga::AllTalam.size() << "\n";
    cout << "Attempting to Initialize Ragas..." << endl;
    efm::InitializeKnownRagas();
    if(argc < 3)
    {
        show_usage(argv[0]);
        return -1;
    }
    else
    {
        std::string sourceExtract{}, sourceProcess{};
        std::string destination{};
        std::string raag{}; // stores the ragaId from the CompMusic Dataset
        std::string notationFile{};
        int num_beats = -32768;
        for(int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if((arg == "-h") || (arg == "--help"))
            {
                show_usage(argv[0]);
                return 0;
            }
            else if((arg == "-d") || (arg == "--destination"))
            {
                if(i + 1 < argc)
                {
                    destination = argv[++i];
                }

            }
            else if((arg == "-x") || (arg == "--extract"))
            {
                sourceExtract = argv[++i];
            }
            else if((arg == "-p") || (arg == "--process"))
            {
                sourceProcess = argv[++i];
            }
            else if((arg == "-fd") || (arg == "--full_dataset"))
            {
                small = false;
            }
            else if((arg == "-rid") || (arg == "--ragaId"))
            {
                raag = argv[++i];
            }
            else if ((arg == "-rf") || (arg == "--read_file") || (arg == "--parse"))
                notationFile = argv[++i];
            else if((arg == "-nb") || (arg == "--num_beats"))
                num_beats = stoi(argv[++i]);
        }

        if(!sourceExtract.empty())
        {
            if (destination.empty())
            {
                std::cerr << "Destination needs to be specified!" << std::endl;
                return -1;
            }
            ret = FilterAndExtractData(sourceExtract, destination, small, raag);
        }
        if(!sourceProcess.empty())
        {
            if(destination.empty())
            {
                destination = sourceProcess;
            }
           ret =  ProcessExtractedData(sourceProcess, destination, small, raag);
        }
        if(!notationFile.empty())
        {
            if(destination.empty())
            {
                fs::path source_parent = fs::path(notationFile).parent_path();
                destination = source_parent;
            }
            if(num_beats > 0)
                ret = ReadNotations(notationFile, destination, num_beats);
            else
                ret = ReadNotations(notationFile, destination, 8);
        }
    }
    return ret;
}

static int ApplyFilter(const char* filename, const char* filter_descr) // Applies the showcqt filter to the file `filename`, along with `filter_descr` as filter params
{
    FilterParams params;
    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* filt_frame = av_frame_alloc();
    int err;

    if(!frame || !filt_frame)
    {
        perror("Could not allocate frame");
        exit(1);
    }

    AV_ERROR_CHECK(err =  open_input_file(filename, &params), open_input);
    AV_ERROR_CHECK(err = init_filters(filter_descr, &params), init_filters);

    while(1)
    {
        if((err = av_read_frame(params.fmtCtx, &packet)) < 0)
        {
            break;
        }

        if (packet.stream_index == params.audio_stream_index)
        {
            err = avcodec_send_packet(params.decCtx, &packet);
            if (err < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Error while sending packet to the decoder\n");
                break;
            }

            while(err >= 0)
            {
                err = avcodec_receive_frame(params.decCtx, frame);
                if (err == AVERROR(EAGAIN) || err == AVERROR_EOF)
                    break;
                else if (err < 0)
                {
                    av_log(NULL, AV_LOG_ERROR, "Error while receiving the frame from the decoder\n");
                    goto end;
                }

                if(err >= 0)
                {
                    /*
                     * push the audio data from the decoded frame to  the filtergraph.
                     * */
                    if(av_buffersrc_add_frame_flags(params.buffersrcCtx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0)
                    {
                        av_log(NULL, AV_LOG_ERROR, "Error while feeding audio to the filtergraph\n");
                        break;
                    }

                    /*
                     * pull filtered audio from the filtergraph
                     * */
                    while (1)
                    {
                        err = av_buffersink_get_frame(params.buffersinkCtx, filt_frame);
                        if(err == AVERROR(EAGAIN) || err == AVERROR_EOF)
                            break;
                        if (err < 0)
                            goto end;

/*
                        for(int i = 0; i < 8; i++)
                        {
                            dat_pkt.data[i] = filt_frame->data[i];
                            dat_pkt.linesize[i] = filt_frame->linesize[i];
                        }
                        dat_pkt.nb_samples = filt_frame->nb_samples;
                        data.push_back(dat_pkt);
                        print_frame(filt_frame);
                        display_frame(filt_frame, params.buffersinkCtx->inputs[0]->time_base, &params);
*/
                        av_frame_unref(filt_frame);
                    }
//                    print_frame(frame);
                    av_frame_unref(frame);
                }

            }
        }

        av_packet_unref(&packet);
    }



end:
    avfilter_graph_free(&params.filterGraph);
    avcodec_free_context(&params.decCtx);
    avformat_close_input(&params.fmtCtx);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);

    if(err < 0 && err != AVERROR_EOF)
    {
        fprintf(stderr, "Error Occurred: %s\n", av_err2str(err));
        return err;
    }


    return err;
}

static int FilterAndExtractData(const std::string& source, const std::string& dest, bool small, const std::string& raga)
{


    tf::Executor executor;
    tf::Taskflow taskflow;

    auto start = ch::high_resolution_clock::now();

    fs::path audioPath = fs::path(source) / "audio";
    fs::path idPath = fs::path(source) / "_info_/ragaId_to_ragaName_mapping.txt";
    fs::path featurePath = fs::path(source) / "features";

    if(fs::exists(idPath))
    {
        vector<efm::Raga> ragas = efm::GetAllRagas(idPath.c_str());
        cout << "Num Ragas found: " << ragas.size() << endl;

    }

    if(fs::exists(featurePath))
    {

    }

    vector<efm::Song> files = GetAudioFiles(audioPath.c_str(), small, raga);
    cout << "Num files found: " << files.size() << endl;




    if(!files.empty())
    {
        for(const auto& i : files)
        {
            cout << "Raga: " << i.ragaId << " Artist: " << i.Artist << endl;
            const char* filename = i.PathToFile.c_str();
            std::string name(i.SongName);
            std::replace(name.begin(), name.end(), ' ', '_');
            char forbidden[] = "'\"~!@#$/,%^&*";
            efm::RemoveCharsFromString(name, forbidden);
            fs::path dataPath = fs::path(dest) / i.ragaId / i.Artist / name;
            cout << dataPath << endl;
            if(!fs::exists(dataPath))
                CreateDirectory(dataPath.c_str());
            else
            {
            }

            string filt_text = "highpass=100, lowpass=4000, aresample=44100, aformat=sample_fmts=s16:channel_layouts=mono, volume=1, showcqt=s=960x540:fcount=1:no_video=1:no_data=0:tc=0.085:storepath=";
            filt_text.append(dataPath);
            cout << filt_text << endl;
#if PARALLEL == 1
                taskflow.emplace([filename, filt_text] () {
#endif
//            cout << "Serial Extraction Activated! \n";
            cout << "File encountered: " << filename << endl;
            ApplyFilter(filename, filt_text.c_str());
#if PARALLEL == 1
                }).name(i.SongName);
#endif
        }
#if PARALLEL == 1
            executor.run(taskflow).wait();
#endif

    }

    auto stop = ch::high_resolution_clock::now();
    auto duration = ch::duration_cast<ch::microseconds>(stop - start);
    cout << "Time taken: " << (double)duration.count() / 1000000 << " seconds" << endl;

    return 0;

}

static int ProcessExtractedData(const std::string& source, const std::string& dest, bool small, const std::string& raga)
{
    tf::Executor executor;
    tf::Taskflow taskflow;

    auto start = ch::high_resolution_clock::now();

    std::string CARNATIC_RAGAID_RAGA_PATH{};
    if (fs::exists("/media/storage/RagaDataset/Carnatic/_info_/ragaId_to_ragaName_mapping.txt"))
       CARNATIC_RAGAID_RAGA_PATH = "/media/storage/RagaDataset/Carnatic/_info_/ragaId_to_ragaName_mapping.txt";

    std::map<std::string, std::string> ragaIdToRagaMap = efm::Raga::InitializeVariables(CARNATIC_RAGAID_RAGA_PATH.c_str());
    vector<efm::Song> songs = GetSongInfos(source.c_str(), small, raga);

    cout << "Num Songs Encountered: " << songs.size() << endl;
    cout << "Total Num Ragas in the dataset: " << ragaIdToRagaMap.size() << '\n';

    for(auto& si: songs)
    {
#if PARALLEL == 1
        taskflow.emplace([&, si]() {
#endif
            efm::Song s = si;
            cout << "Encountered " << s.SongName << endl;
            if(ragaIdToRagaMap.find(s.ragaId) != ragaIdToRagaMap.end())
                cout << "Detected Raga: " << s.ragaId << ragaIdToRagaMap.at(s.ragaId) << '\n';
            else
                cout << "Detected Raga: " << s.ragaId << '\n';


        efm::ReadDataFiles(s);
            // Assuming that the most occuring note is the fundamental frequency. sum_MidiData calulates the frequency of every note from E0 - A7
            auto sum_v = s.sum_MidiData;
            auto melody = s.MelodyData;

//
//        for (auto i : sum_v) {
//            std::cout << i.index << "  " << i.value << "\t";
//
//        }
//        std::cout << std::endl;


//        auto indices = efm::GetIndicesFromDataItems(sum_v);
//        for (auto i : indices) {
//            std::cout << i << " ";
//        }
//        std::cout << "\n";


            auto f0 = efm::GetFundamentalFrequency(sum_v, s);
            s.SetF0(f0.index);
            s.WriteEncodedSequenceToFile();
            cout << "Done " << s.SongName << endl;
#if PARALLEL == 1
        delete &s;

        }).name(si.SongName);
#endif
    }
#if PARALLEL == 1
    executor.run_n(taskflow, 8).wait();
#endif
    std::cout << "Completed!! \n";
    auto stop = ch::high_resolution_clock::now();
    auto duration = ch::duration_cast<ch::microseconds>(stop - start);
    cout << "Time taken: " << (double)duration.count() / 1000000 << " seconds" << endl;

    return 0;
}

static int CreateDataset(const std::string& source, const std::string& ragaId, const std::string& dest)
{
//    std::vector<efm::Song> songs = GetAudioFiles(source.c_str());


    return 0;
}

static int ReadNotations(const std::string& filename, const std::string& destination_folder, const int num_beats)
{
    efm::Song s;
    s.SetCountsPerBeat(num_beats);
    s.ReadNotationsFromFile(filename);
    s.WriteParsedNotationsToFile(destination_folder);
    return 0;
}
