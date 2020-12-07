#include <iostream>
#include <filesystem>
#include <chrono>
#include <string>
#include "../inc/filter.hpp"
#include "../inc/fileops.hpp"
#include "taskflow/taskflow.hpp"
//const char *filter_descr = "lowpass=frequency=8000 [b]; [b] aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono[c]; [c]showcqt=basefreq=65.4064:endfreq=987.77:no_video=1:storepath=/media/theko/UsbStorage1/cqt/temp.txt"; //showcqt=tc=0.17:tlength='st(0,0.17); 384*tc / (384 / ld(0) + tc*f /(1-ld(0))) + 384*tc / (tc*f / ld(0) + 384 /(1-ld(0)))':sono_v=a_weighting(f)*16:sono_h=980"; // dynaudnorm [a]; [a] lowpass=frequency=4000 [b]; [b] aresample=4000,aformat=sample_fmts=s16:channel_layouts=mono[c]; [c]
//const char *filter_descr = "aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono"; s=192x108:
namespace fs = std::filesystem;
namespace ch = std::chrono;



struct AudioData{
    uint8_t* data[8]{};
    int linesize[8]{};
    int nb_samples{};
};


static int ApplyFilter(const char* filename, const char* filter_descr);

int main(int argc, char** argv) {

    vector<AudioData> data{};
    AudioData dat_pkt{};

    tf::Executor executor;
    tf::Taskflow taskflow;

    fs::path dataRoot = "/media/theko/UsbStorage1/cqt/";

    if (argc == 2)
    {
/*
        const fs::path pathToShow = argv[1];
        std::cerr << "exists() = " << fs::exists(pathToShow) << std::endl
                  << "root_name() = " << pathToShow.root_name() << "\n"
                  << "root_path() = " << pathToShow.root_path() << "\n"
                  << "relative_path() = " << pathToShow.relative_path() << "\n"
                  << "parent_path() = " << pathToShow.parent_path() << "\n"
                  << "filename() = " << pathToShow.filename() << "\n"
                  << "stem() = " << pathToShow.stem() << "\n"
                  << "extension() = " << pathToShow.extension() << "\n";
        try
        {
            std::cout << "cannonical() = " << fs::canonical(pathToShow) << pathToShow.string().c_str() << "\n";
        }
        catch (fs::filesystem_error err)
        {
            std::cout << "exception: " << err.what() << "\n";
        }
*/

        vector<fs::path> files = GetFilesInPath(argv[1]);
        cout << "received Vector size: " << files.size() << endl;

        int iter = 0;
        auto start = ch::high_resolution_clock::now();

        while(iter < files.size())
        {
            const char* e = files[iter].c_str();
            string filt_text = "lowpass=frequency=8000 [b]; [b] aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono[c]; [c]showcqt=no_video=1:storepath=";
            string datFileName = dataRoot.string().append(files[iter].stem().concat("_cqt.txt"));
            filt_text.append(datFileName);
//            cout << "Final Format string: " << filt_text << endl;
            taskflow.emplace([e, filt_text] () {
                cout << "File encountered: " << e << endl;
                ApplyFilter(e, filt_text.c_str());
            });
            iter++;
        }

        cout << "All Threads at work? \n";
        executor.run(taskflow).wait();
        auto stop = ch::high_resolution_clock::now();
        auto duration = ch::duration_cast<ch::microseconds>(stop - start);

//        cout << "size of the data buffer: " << data.capacity() << " number of elements: " << data.size() << endl;
        cout << "time taken: " << duration.count() << " microseconds" << endl;
//    cout << "Hardware Concurrency: " << thread::hardware_concurrency() << endl;


        exit(1);
    }



//    ApplyFilter("/home/theko/songs/Gat1-000.wav");

    return 0;
}

static int ApplyFilter(const char* filename, const char* filter_descr)
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
