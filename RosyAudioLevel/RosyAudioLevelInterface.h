#pragma once
#include <cstdio>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <cmath>
#include "kiss_fft130/kiss_fftr.h"
#include <cassert>
#include <string>
using namespace std;

#define MAX_AUDIO_FRAME_SIZE 192000

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

struct Measure
{
	enum Port {
		PORT_OUTPUT,
		PORT_INPUT,
	};
	enum Channel {
		CHANNEL_FL,
		CHANNEL_FR,
		CHANNEL_C,
		CHANNEL_LFE,
		CHANNEL_BL,
		CHANNEL_BR,
		CHANNEL_SL,
		CHANNEL_SR,
		MAX_CHANNELS,
		CHANNEL_SUM = MAX_CHANNELS
	};
	enum Type {
		TYPE_RMS,
		TYPE_PEAK,
		TYPE_FFT,
		TYPE_BAND,
		TYPE_FFTFREQ,
		TYPE_BANDFREQ,
		TYPE_FORMAT,
		TYPE_DEV_STATUS,
		TYPE_DEV_NAME,
		TYPE_DEV_ID,
		TYPE_DEV_LIST,
		// ... //
		NUM_TYPES
	};
	enum Format {
		FMT_INVALID,
		FMT_PCM_S16,
		FMT_PCM_F32,
		// ... //
		NUM_FORMATS
	};
	struct BandInfo {
		float	freq;
		float	x;
	};

	Port					m_port;						// port specifier (parsed from options)
	Channel					m_channel;					// channel specifier (parsed from options)
	Type					m_type;						// data type specifier (parsed from options)
	Format					m_format;					// format specifier (detected in init)
	int						m_envRMS[2];				// RMS attack/decay times in ms (parsed from options)
	int						m_envPeak[2];				// peak attack/decay times in ms (parsed from options)
	int						m_envFFT[2];				// FFT attack/decay times in ms (parsed from options)
	int						m_fftSize;					// size of FFT (parsed from options)
	int						m_fftOverlap;				// number of samples between FFT calculations
	int						m_fftIdx;					// FFT index to retrieve (parsed from options)
	int						m_nBands;					// number of frequency bands (parsed from options)
	int						m_bandIdx;					// band index to retrieve (parsed from options)
	double					m_gainRMS;					// RMS gain (parsed from options)
	double					m_gainPeak;					// peak gain (parsed from options)
	double					m_freqMin;					// min freq for band measurement
	double					m_freqMax;					// max freq for band measurement
	double					m_sensitivity;				// dB range for FFT/Band return values (parsed from options)
	Measure* m_parent;					// parent measure, if any
	void* m_skin;						// skin pointer
	LPCWSTR					m_rmName;					// measure name
	IMMDeviceEnumerator* m_enum;						// audio endpoint enumerator
	IMMDevice* m_dev;						// audio endpoint device
	WAVEFORMATEX* m_wfx;						// audio format info
	IAudioClient* m_clAudio;					// audio client instance
	IAudioCaptureClient* m_clCapture;				// capture client instance
	WCHAR					m_reqID[64];				// requested device ID (parsed from options)
	WCHAR					m_devName[64];				// device friendly name (detected in init)
	float					m_kRMS[2];					// RMS attack/decay filter constants
	float					m_kPeak[2];					// peak attack/decay filter constants
	float					m_kFFT[2];					// FFT attack/decay filter constants
	double					m_rms[MAX_CHANNELS];		// current RMS levels
	double					m_peak[MAX_CHANNELS];		// current peak levels
	double					m_pcMult;					// performance counter inv frequency
	LARGE_INTEGER			m_pcFill;					// performance counter on last full buffer
	LARGE_INTEGER			m_pcPoll;					// performance counter on last device poll
	kiss_fftr_cfg			m_fftCfg[MAX_CHANNELS];		// FFT states for each channel
	float* m_fftIn[MAX_CHANNELS];		// buffer for each channel's FFT input
	float* m_fftOut[MAX_CHANNELS];		// buffer for each channel's FFT output
	float* m_fftKWdw;					// window function coefficients
	float* m_fftTmpIn;					// temp FFT processing buffer
	kiss_fft_cpx* m_fftTmpOut;				// temp FFT processing buffer
	int						m_fftBufW;					// write index for input ring buffers
	int						m_fftBufP;					// decremental counter - process FFT at zero
	float* m_bandFreq;					// buffer of band max frequencies
	float* m_bandOut[MAX_CHANNELS];	// buffer of band values

	Measure()
		: m_port(PORT_OUTPUT)
		, m_channel(CHANNEL_SUM)
		, m_type(TYPE_RMS)
		, m_format(FMT_INVALID)
		, m_fftSize(0)
		, m_fftOverlap(0)
		, m_fftIdx(-1)
		, m_nBands(0)
		, m_bandIdx(-1)
		, m_gainRMS(1.0)
		, m_gainPeak(1.0)
		, m_freqMin(20.0)
		, m_freqMax(20000.0)
		, m_sensitivity(35.0)
		, m_parent(NULL)
		, m_skin(NULL)
		, m_rmName(NULL)
		, m_enum(NULL)
		, m_dev(NULL)
		, m_wfx(NULL)
		, m_clAudio(NULL)
		, m_clCapture(NULL)
		, m_fftKWdw(NULL)
		, m_fftTmpIn(NULL)
		, m_fftTmpOut(NULL)
		, m_fftBufW(0)
		, m_fftBufP(0)
		, m_bandFreq(NULL)
	{
		m_envRMS[0] = 300;
		m_envRMS[1] = 300;
		m_envPeak[0] = 50;
		m_envPeak[1] = 2500;
		m_envFFT[0] = 300;
		m_envFFT[1] = 300;
		m_reqID[0] = '\0';
		m_devName[0] = '\0';
		m_kRMS[0] = 0.0f;
		m_kRMS[1] = 0.0f;
		m_kPeak[0] = 0.0f;
		m_kPeak[1] = 0.0f;
		m_kFFT[0] = 0.0f;
		m_kFFT[1] = 0.0f;
		for (int iChan = 0; iChan < MAX_CHANNELS; ++iChan) {
			m_rms[iChan] = 0.0;
			m_peak[iChan] = 0.0;
			m_fftCfg[iChan] = NULL;
			m_fftIn[iChan] = NULL;
			m_fftOut[iChan] = NULL;
			m_bandOut[iChan] = NULL;
		}
		LARGE_INTEGER pcFreq;
		QueryPerformanceFrequency(&pcFreq);
		m_pcMult = 1.0 / (double)pcFreq.QuadPart;
	}

};

#define WINDOWS_BUG_WORKAROUND	1
#define REFTIMES_PER_SEC		10000000
#define TWOPI					(2*3.14159265358979323846)
#define EXIT_ON_ERROR(hres)		if(FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(p)			if((p)!=NULL) { (p)->Release(); (p) = NULL; }
#define CLAMP01(x)				max(0.0, min(1.0, (x)))

#define EMPTY_TIMEOUT			0.500
#define DEVICE_TIMEOUT			1.500
#define QUERY_TIMEOUT			(1.0/60)

bool InitializeAudioInterface();
wstring CaptureAudioData(int nFlag);