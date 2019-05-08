#include "AudioManager.h"
#include <memory>
#include <xaudio2.h>
#include "WAVFileReader.h"
#include "Managers/SceneManager.h"
#include <mutex>
#pragma comment(lib,"xaudio2.lib")
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

IXAudio2* AudioManager::pXAudio2 = nullptr;
IXAudio2MasteringVoice* AudioManager::pMasterVoice = nullptr;
WAVEFORMATEXTENSIBLE AudioManager::wfx = { 0 };
std::map<std::string, std::shared_ptr<AudioManager::AudioData>> AudioManager::buffers;
IXAudio2SourceVoice* AudioManager::pSourceVoice = nullptr;
bool AudioManager::StopMusic = false;
std::condition_variable AudioManager::cv;
HRESULT AudioManager::PlaySound(const std::string& fileName, bool infiniteLoop)
{
	StopMusic = false;
	HRESULT hr;



	std::shared_ptr<AudioData> data = GetBuffer(fileName);

	if (!data)
	{
		SceneManager::threadPool->AddWork([fileName, infiniteLoop]() {AudioManager::ManageSounds(fileName, infiniteLoop); });
		return S_OK;
	}

	if (infiniteLoop)
	{
		data->buffer.LoopBegin = data->wavData.loopStart;
		data->buffer.LoopLength = data->wavData.loopLength;
		data->buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else
	{
		data->buffer.LoopBegin = data->wavData.loopStart;
		data->buffer.LoopLength = data->wavData.loopLength;
		data->buffer.LoopCount = 0;
	}
	XAUDIO2_VOICE_STATE state;
	data->sourceVoice->GetState(&state);
	if (state.BuffersQueued == 0)
	{

		if (FAILED(hr = data->sourceVoice->SubmitSourceBuffer(&data->buffer)))
			return hr;
		//data->sourceVoice->Stop();
		if (FAILED(hr = data->sourceVoice->Start(0)))
			return hr;
	}
	return S_OK;



}

HRESULT AudioManager::ManageSounds(const std::string& fileName, bool infiniteLoop)
{
	HRESULT hr;

	std::unique_ptr<uint8_t[]> waveFile;
	std::wstring wide_string = std::wstring(fileName.begin(), fileName.end());
	const wchar_t* result = wide_string.c_str();


	std::shared_ptr<AudioData> data = std::make_shared<AudioData>();
	if (FAILED(hr = WAVFileReader::LoadWAVAudioFromFileEx(result, waveFile, data->wavData)))
	{
		wprintf(L"Failed reading WAV file: %#X (%s)\n", hr, result);
		return hr;
	}

	//
	// Play the wave using a XAudio2SourceVoice
	//
	//// Create the source voice
	data->buffer = { 0 };
	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	data->buffer.pAudioData = data->wavData.startAudio;
	data->buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	data->buffer.AudioBytes = data->wavData.audioBytes;

	if (data->wavData.loopLength > 0)
	{
		data->buffer.LoopBegin = data->wavData.loopStart;
		data->buffer.LoopLength = data->wavData.loopLength;
		data->buffer.LoopCount = 1; // We'll just assume we play the loop twice
	}

	if (infiniteLoop)
	{
		data->buffer.LoopBegin = data->wavData.loopStart;
		data->buffer.LoopLength = data->wavData.loopLength;
		data->buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	if (FAILED(hr = pXAudio2->CreateSourceVoice(&data->sourceVoice, data->wavData.wfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		return hr;
	}

	buffers.emplace(fileName, data);

	if (FAILED(hr = data->sourceVoice->SubmitSourceBuffer(&data->buffer)))
		return hr;

	if (FAILED(hr = data->sourceVoice->Start(0)))
		return hr;

	std::mutex lock;
	std::unique_lock<std::mutex> lk(lock);
	cv.wait(lk);
	data->sourceVoice->Stop();
	data->sourceVoice->DestroyVoice();
	return hr;
}

void AudioManager::StopSounds()
{
	StopMusic = true;
	cv.notify_all();
	for (auto it = buffers.begin(); it != buffers.end(); )
		it = buffers.erase(it);

}

HRESULT AudioManager::Initialize()
{
	auto hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

	if (FAILED(hr))
		return hr;

	hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);

	if (FAILED(hr))
		return hr;

}

std::shared_ptr<AudioManager::AudioData> AudioManager::GetBuffer(const std::string& fileName)
{
	const auto itr = buffers.find(fileName);

	if (itr == buffers.end())
		return nullptr; // No scene with sceneName found
	return itr->second;

}

