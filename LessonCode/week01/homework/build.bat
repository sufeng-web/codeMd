cd llama
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
ren build\bin\Release\llama-cli.exe localllm.exe
#build\bin\Release\localllm.exe -m models\qwen2.5-1.5b-instruct-q4_0.gguf 