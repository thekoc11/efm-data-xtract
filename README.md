# `efm-data-xtract`: A Data Extractor for the Early Fusion Music Project

This extractor can parse Carnatic compositions in any text format, and extract notations from music audio source(s) in a 12-tonal numerical format. [My Fork of FFMPEG](https://github.com/thekoc11/FFmpeg) will allow you to view the extracted notations as a video.  

For parsing text notations, please remember to add the `<BEGIN>` and the `<END>` tags in the text file, the former a line before the desired location for beginning the parse and the latter a line after the desired location of ending the parse. 

Usage:
```
-x, --extract SOURCE            extract data mode. SOURCE is a directory which contains audios in SOURCE/audio and other essential conversion files in the SOURCE/_info_ subfolders
-p, --process SOURCE            process data mode. SOURCE here needs to be the DESTINATION storing the results from the --extract option
-d, --destination DESTINATION   Specify the destination path for the output current operation
-fd, --full_dataset             Use the full (audio) dataset. This refers to the usage of the full CompMusic Raga Dataset. Disabled by default
-h, --help                      Display this help message
-rf, --parse FILENAME.txt       Read and interpret the textual notations given in the contents of FILENAME. In case the destination is not specified using -d, the final output 
                                FILENAME-parsed.txt is saved in the same folder
-nb, --num_beats X              IMPORTANT: USE THIS WHILE USING -rf WITHOUT FAIL; Number of beats in the concerned song (used for parsing). The default value 8. So, in case -nb is not specified along side -rf, the parser assumes a 8-count beat, like the Adi Talam in Carnatic or Taal Keherwa in Hindustani.
```

## Building from source

Before building this repo, you'll first have to build [FFMPEG](https://github.com/thekoc11/FFmpeg). This is critical, as it stores the LibAV* files to a specific folder that this project relies on. 

After building FFMpeg, clone this repository to a folder of your choice:
```git
git clone https://github.com/thekoc11/efm-data-xtract.git 
```
Make sure you have CMake 3.16 installed on your sytem before building this project.

Then, to build this project, run the following:
```
cd efm-data-xtract
mkdir build-debug
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --target proj -- -j 25
```
This builds the final application. For those not very familiar with `cmake` usages, `-DCMAKE_BUILD_TYPE` configures the build type (here, Debug), `--target` requires the name of the project (listed in CMakeLists.txt) and `-j` is the number of "jobs" or number of threads for on which the building process is run (it is usually a good practice in multicore architectures to leave around a fifth of the cores for OS function. We have a 32-core CPU, hence we left 7 cores for the OS and are using 25 cores for the compilation). This massively optimizes the build time.
Finally, to run the parser:
```
cd build-debug
./proj
```
This should output a message similar to the one given in the introductory section above on the console.
