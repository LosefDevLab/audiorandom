//AUR SOURCE CODE
//AUDIO TECHNOLOGY BY PORTAUDIO
#include <iostream>
#include <fstream>
#include <random>
#include <portaudio.h>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <thread>

using namespace std;

const int SAMPLE_RATE = 44100;               // 采样率
const int FRAMES_PER_BUFFER = 256;           // 每次缓冲的音频帧数
const int NUM_CHANNELS = 1;                  // 音频通道数
atomic<bool> isRunning(true);            // 控制程序是否继续运行

void OutToFile(int random_number)
{
    static int count = 0;
    ofstream outFile("rn.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << random_number << " ";
        count++;

        if (count >= 20)
        {
            outFile << endl;
            count = 0;
        }
    }
    else
    {
        cerr << "[E]Unable to open file rn.txt for writing." << endl;
    }
}

// 分析音频 并利用音频数据生成随机数
static int recordCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData)
{
    // 获取输入音频数据
    const float *input = static_cast<const float *>(inputBuffer);

    // 使用音频数据生成随机数
    float sum = 0;
    for (unsigned long i = 0; i < framesPerBuffer; ++i)
    {
        sum += abs(input[i]);  // 计算音频数据的总振幅
    }

    // 根据音频的总振幅生成随机数种子
    unsigned int audioSeed = static_cast<unsigned int>(sum * 2147483647); // 使用振幅作为随机数种子

    // 获取当前的时间戳生成随机数种子
    unsigned int timestampSeed = static_cast<unsigned int>(
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count());

    // 合并音频种子和时间戳种子
    unsigned int finalSeed = audioSeed + timestampSeed;

    
    default_random_engine generator(finalSeed);
    uniform_int_distribution<int> distribution(0, 2147483647);
    int random_number = distribution(generator);

    cout << random_number << " " << flush;

    OutToFile(random_number);

    return isRunning.load() ? paContinue : paComplete;
}

int main()
{
    cout << "XYLCS XIT LosefDevLab create. Audio technology by PortAudio\n  Audio Random - AUR \n    ver1.0.r1.b6 \nPress Enter to start, Ctrl+C to stop." << endl;
    cin.get();

    // 初始化
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        cerr << "[PortAudio] initialization error: " << Pa_GetErrorText(err) << endl;
        return 1;
    }

    // 打开音频流
    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, NUM_CHANNELS, 0, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, recordCallback, nullptr);
    if (err != paNoError)
    {
        cerr << "[PortAudio stream] opening error: " << Pa_GetErrorText(err) << endl;
        Pa_Terminate();
        return 1;
    }

    // 开始录音
    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        cerr << "[PortAudio stream] start error: " << Pa_GetErrorText(err) << endl;
        Pa_Terminate();
        return 1;
    }

    cout << "[i]Recording started. Generating random numbers...\n[o]random number:" << endl;
    this_thread::sleep_for(chrono::seconds(3));

    while (isRunning.load())
    {
        this_thread::sleep_for(chrono::milliseconds(100)); // 保持程序活跃
    }

    // 停止并关闭音频流
    err = Pa_StopStream(stream);
    if (err != paNoError)
    {
        cerr << "[PortAudio stream] stop error: " << Pa_GetErrorText(err) << endl;
    }
    err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
        cerr << "[PortAudio stream] close error: " << Pa_GetErrorText(err) << endl;
    }

    Pa_Terminate();

    return 0;
}
