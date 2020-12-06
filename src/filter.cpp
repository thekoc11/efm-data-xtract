//
// Created by theko on 28/11/20.
//

#include "filter.hpp"



int open_input_file(const char* filename, FilterParams* params)
{
    int ret;
    AVCodec* dec;

    AV_ERROR_CHECK(avformat_open_input(&params->fmtCtx, filename, NULL, NULL), avformat_open_input);
    AV_ERROR_CHECK(avformat_find_stream_info(params->fmtCtx, NULL), find_stream_info);
    ret = av_find_best_stream(params->fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    AV_ERROR_CHECK(ret, find_best_stream);
    params->audio_stream_index = ret;

    params->decCtx = avcodec_alloc_context3(dec);

    if (!params->decCtx)
        return AVERROR(ENOMEM);

    avcodec_parameters_to_context(params->decCtx, params->fmtCtx->streams[params->audio_stream_index]->codecpar);
    AV_ERROR_CHECK(avcodec_open2(params->decCtx, dec, NULL), params_to_codec_context);

    return 0;
}
int init_filters(const char* filters_descr, FilterParams* params)
{
    char args[512];
    int ret = 0;
    const AVFilter* abuffersrc = avfilter_get_by_name("abuffer");
    const AVFilter* abuffersink = avfilter_get_by_name("abuffersink");
    const AVFilter* buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut* outputs = avfilter_inout_alloc();
    AVFilterInOut* inputs = avfilter_inout_alloc();
    static const enum AVSampleFormat out_sample_fmts[]= {AV_SAMPLE_FMT_S16, static_cast<const AVSampleFormat>(-1)};
    static const int64_t out_channel_layouts[] = {AV_CH_LAYOUT_MONO, -1};
    static const int out_sample_rates[] = {8000, 16000, 24000, 44100, 48000, -1};
    const AVFilterLink* outlink;
    AVRational timeBase = params->fmtCtx->streams[params->audio_stream_index]->time_base;
    enum AVPixelFormat pixFmts[] = {AV_PIX_FMT_AYUV64LE, AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE};

    params->filterGraph = avfilter_graph_alloc();
    if(!params->filterGraph || !outputs || !inputs)
    {
        ret = AVERROR(ENOMEM);
        goto end;
    }

//    buffer audio source: decoded  frames from the decoder will be inserted here
    if(!params->decCtx->channel_layout)
        params->decCtx->channel_layout = av_get_default_channel_layout(params->decCtx->channels);

    snprintf(args, sizeof(args),
             "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
             timeBase.num, timeBase.den, params->decCtx->sample_rate,
             av_get_sample_fmt_name(params->decCtx->sample_fmt), params->decCtx->channel_layout);

    AV_ERROR_CHECK(avfilter_graph_create_filter(&params->buffersrcCtx, abuffersrc, "in",
                                                args, NULL, params->filterGraph), graph_create_filter_in);

//  buffer audio sink: to terminate the audio filter chain
    AV_ERROR_CHECK(avfilter_graph_create_filter(&params->buffersinkCtx, buffersink, "out",
                                                NULL, NULL, params->filterGraph), graph_create_filter_out);

//    AV_ERROR_CHECK(av_opt_set_int_list(params->buffersinkCtx, "sample_fmts", out_sample_fmts,
//                                       -1, AV_OPT_SEARCH_CHILDREN), set_opt_sample_fmts);
//    AV_ERROR_CHECK(av_opt_set_int_list(params->buffersinkCtx, "channel_layouts", out_channel_layouts,
//                                       -1, AV_OPT_SEARCH_CHILDREN), set_opt_ch_layouts);
//    AV_ERROR_CHECK(av_opt_set_int_list(params->buffersinkCtx, "sample_rates", out_sample_rates,
//                                       -1, AV_OPT_SEARCH_CHILDREN), set_opt_sample_rates);
    AV_ERROR_CHECK(av_opt_set_int_list(params->buffersinkCtx, "pix_fmts", pixFmts,
                                       AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN), set_pix_fmts);

    /*
     * Set endpoints for the filter graph. The filter_graph will be linked to the graph
     * described by filters_descr(argument to this function)
     * */

    /*
     * The buffer source output must be connected to the input pad of the first filter described by @filters_descr;
     * since the first filter input label is not specified, it is set to "in" by default.
     * */

    outputs->name       = av_strdup("in");
    outputs->filter_ctx = params->buffersrcCtx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of the last filter described by @filters_descr;
     * since the last filter output label is not defined, it is set to "out" by default.
     * */
    inputs->name        = av_strdup("out");
    inputs->filter_ctx  = params->buffersinkCtx;
    inputs->pad_idx     = 0;
    inputs->next        = NULL;

    AV_ERROR_CHECK(avfilter_graph_parse_ptr(params->filterGraph, filters_descr,
                                            &inputs, &outputs, NULL), parse_filtergraph_ptr);
    AV_ERROR_CHECK(avfilter_graph_config(params->filterGraph, NULL), config_filtergraph);

    /*
     * Print the summary of the sink buffer
     * Note: @args buffer is reused to store channel layout string
     * */
//
//    outlink = params->buffersinkCtx->inputs[0];
//    av_get_channel_layout_string(args, sizeof(args), -1, outlink->channel_layout);
//    av_log(NULL, AV_LOG_INFO, "Output: srate: %dHz, fmt: %s, chlayout: %s\n",
//           (int)outlink->sample_rate,
//           (char*)av_x_if_null(av_get_sample_fmt_name(static_cast<AVSampleFormat>(outlink->format)), "?"),
//           args);

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
}

extern "C" void print_frame(const AVFrame* frame)
{
    const int n = frame->nb_samples * av_get_channel_layout_nb_channels(frame->channel_layout);
    const uint16_t *p   = (uint16_t*) frame->data[0];
    const  uint16_t *p_end = p + n;

    while(p < p_end)
    {
        fputc(*p    & 0xff, stdout);
        fputc(*p>>8    & 0xff, stdout);
        p++;
    }
    fflush(stdout);
}

void display_frame(const AVFrame* frame, AVRational time_base, FilterParams* params)
{
    int x, y;
    uint8_t *p0, *p;
    int64_t delay;

    if(frame->pts != AV_NOPTS_VALUE)
    {
        if(params->last_pts != AV_NOPTS_VALUE)
        {
            /*
             * Sleep roughly for the right amount of time;
             * usleep is in microseconds, just like AV_TIME_BASE*/
            delay = av_rescale_q(frame->pts - params->last_pts,
                                 time_base, AV_TIME_BASE_Q);
            if(delay > 0 && delay < 1000000)
            {
                usleep(delay);
            }
            params->last_pts = frame->pts;
        }
    }

//    trivial ASCII grayscale display
    p0 = frame->data[0];
    puts("\033c");
    for(y = 0; y < frame->height; y++)
    {
        p = p0;
        for(x= 0; x < frame->width; x++)
            putchar(".-+#" [*(p++) / 52]);
        putchar('\n');
        p0 += frame->linesize[0];
    }
    fflush(stdout);

}
