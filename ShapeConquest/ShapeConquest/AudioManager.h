#pragma once
#include <xaudio2.h>
#include <string>
#include <unordered_map>
#include "WAVFileReader.h"
#include <mutex>
#include <map>

class AudioManager
{
	struct AudioData
	{
		WAVFileReader::WAVData wavData;
		IXAudio2SourceVoice* sourceVoice;
		XAUDIO2_BUFFER buffer;
	};
public:
	static HRESULT Initialize();
	static HRESULT LoadAudioFile(const std::string& fileName);


	static IXAudio2* pXAudio2;
	static IXAudio2MasteringVoice* pMasterVoice;

	static HRESULT PlaySound(const std::string& fileName, bool infiniteLoo = false);
	static HRESULT ManageSounds(const std::string& fileName, bool infiniteLoop);
	static void StopSounds();
private:
	static	std::shared_ptr<AudioData> GetBuffer(const std::string& fileName);
	static WAVEFORMATEXTENSIBLE wfx;
	static IXAudio2SourceVoice* pSourceVoice;
	static std::map<std::string, std::shared_ptr<AudioData>> buffers;
	static bool StopMusic;
	static std::condition_variable cv;
};

