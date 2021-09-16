# `efm-data-xtract`: A Data Extractor for the Early Fusion Music Project

This extractor can parse Carnatic compositions in any text format, and extract notations from music audio source(s) in a 12-tonal numerical format. [My Fork of FFMPEG](https://github.com/thekoc11/FFmpeg) will allow you to view the extracted notations as a video.  

For parsing text notations, please remember to add the `<BEGIN>` and the `<END>` tags in the text file, the former a line before the desired location for beginning the parse and the latter a line after the desired location of ending the parse. 

Man Pages:
```
-x, --extract SOURCE            extract data mode. SOURCE is a directory which contains audios in SOURCE/audio and other essential conversion files in the SOURCE/_info_ subfolders
-p, --process SOURCE            process data mode. SOURCE here needs to be the DESTINATION storing the results from the --extract option
-d, --destination DESTINATION   Specify the destination path for the output current operation
-fd, --full_dataset             Use the full (audio) dataset. This refers to the usage of the full CompMusic Raga Dataset. Disabled by default
-h, --help                      Display this help message
-rf, --parse FILENAME.txt       Read and interpret the textual notations given in the contents of FILENAME. In case the destination is not specified using -d, the final output 
                                FILENAME-parsed.txt is saved in the same folder
-nb, --num_beats X              IMPORTANT: USE THIS WHILE USING -rf WITHOUT FAIL; Number of beats in the concerned song (used for parsing). The default value 6. So, in case -nb is not specified along side -rf, the parser assumes a 6-count beat, like the Rupakam Talam in Carnatic or Taal Dadra in Hindustani.
```

## Building from source

--- To be updated shortly ---
