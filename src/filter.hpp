//
// Created by theko on 28/11/20.
//

#ifndef PROJ_FILTER_HPP
#define PROJ_FILTER_HPP

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
#include <unistd.h>

void print_frame(const AVFrame* frame);

};

#include <cstdlib>
#ifdef av_err2str
#undef av_err2str
av_always_inline char* av_err2str(int errnum)
{
    // static char str[AV_ERROR_MAX_STRING_SIZE];
    // thread_local may be better than static in multi-thread circumstance
    thread_local char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#endif
#define Name_OF( v ) #v
#define AV_ERROR_CHECK(e, pr_Str) \
        if (e < 0)        \
        {                 \
          char err_str[64] = {};\
          av_log(NULL, AV_LOG_ERROR, "Command failed with exit code %d: %s | %s \n", e, av_err2str(e), Name_OF(pr_Str)); \
          exit(e);\
        }

struct FilterParams{

    AVFormatContext *fmtCtx{};
    AVCodecContext *decCtx{};
    AVFilterContext *buffersinkCtx{};
    AVFilterContext *buffersrcCtx{};
    AVFilterGraph *filterGraph{};
    int audio_stream_index = -1;
    int64_t last_pts = AV_NOPTS_VALUE;
};
int open_input_file(const char* filename, FilterParams* params);
int init_filters(const char* filters_descr, FilterParams* params);
void display_frame(const AVFrame* frame, AVRational time_base, FilterParams* params);


#endif //PROJ_FILTER_HPP
