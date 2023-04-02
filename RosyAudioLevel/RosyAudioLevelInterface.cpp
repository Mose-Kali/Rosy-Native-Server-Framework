#include "pch.h"
#include "RosyAudioLevelInterface.h"

BYTE* buffer;
UINT32 nFrames;
DWORD flags;
UINT64 pos;
Measure Ms;
Measure* m = &Ms;

bool InitializeAudioInterface() {
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
	const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
	IAudioClient* _AudioClient;
	IAudioCaptureClient* _CaptureClient;
	IMMDevice* _Device;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	HANDLE _AudioSamplesReadyEvent = NULL;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		printf("Unable to retrieve CoCreateInstance %x\n", hr);
		goto Exit;
	}
	//这里可以调用EnumAudioEndpoints选择使用其它设备
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_Device);
	if (FAILED(hr))
	{
		printf("Unable to retrieve device %x\n", hr);
		goto Exit;
	}
	SafeRelease(&deviceEnumerator);
	{
		_Device->AddRef();    // Since we're holding a copy of the endpoint, take a reference to it.  It'll be released in Shutdown();

		_AudioSamplesReadyEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
		if (_AudioSamplesReadyEvent == NULL)
		{
			printf("Unable to create samples ready event: %d.\n", GetLastError());
			return false;
		}

		hr = _Device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&_AudioClient);
		if (FAILED(hr))
		{
			printf("Unable to activate audio client: %x.\n", hr);
			return false;
		}

		WAVEFORMATEX* _MixFormat;
		UINT32              _BufferSize;
		hr = _AudioClient->GetMixFormat(&_MixFormat);
		if (FAILED(hr))
		{
			printf("Unable to get mix format on audio client: %x.\n", hr);
			return false;
		}
		IAudioRenderClient* clRender = NULL;
		{
			hr = _AudioClient->Initialize(
				AUDCLNT_SHAREMODE_SHARED,
				0,
				REFTIMES_PER_SEC,
				0,
				_MixFormat,
				NULL
			);
			EXIT_ON_ERROR(hr)
				// get the frame count
				UINT32 nFrames;
			hr = _AudioClient->GetBufferSize(&nFrames);
			EXIT_ON_ERROR(hr)
				// create a render client
				hr = _AudioClient->GetService(IID_IAudioRenderClient, (void**)&clRender);
			EXIT_ON_ERROR(hr)
				// get the buffer
				BYTE* buffer;
			hr = clRender->GetBuffer(nFrames, &buffer);
			EXIT_ON_ERROR(hr)
				// release it
				hr = clRender->ReleaseBuffer(nFrames, AUDCLNT_BUFFERFLAGS_SILENT);
			EXIT_ON_ERROR(hr)
				// release the render client
				clRender->Release();
			clRender = NULL;
			// release the audio client
			_AudioClient->Release();
			// create a new IAudioClient
			hr = _Device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&_AudioClient);
		}
		EXIT_ON_ERROR(hr)
			size_t _FrameSize = (_MixFormat->wBitsPerSample / 8) * _MixFormat->nChannels;
		WAVEFORMATEX* wfex2;
		HRESULT res = _AudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, _MixFormat, &wfex2);
		//InitializeAudioEngine
		hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 20 * 10000, 0, _MixFormat, NULL);

		if (FAILED(hr))
		{
			printf("Unable to initialize audio client: %x.\n", hr);
			return false;
		}

		//
		//  Retrieve the buffer size for the audio client.
		//
		/*hr = _AudioClient->GetBufferSize(&_BufferSize);
		if (FAILED(hr))
		{
			printf("Unable to get audio client buffer: %x. \n", hr);
			return false;
		}
		hr = _AudioClient->SetEventHandle(_AudioSamplesReadyEvent);
		if (FAILED(hr))
		{
			printf("Unable to set ready event: %x.\n", hr);
			return false;
		}*/

		hr = _AudioClient->GetService(IID_IAudioCaptureClient, (void**)(&_CaptureClient));
		if (FAILED(hr))
		{
			printf("Unable to get new capture client: %x.\n", hr);
			return false;
		}

		//开始采集

		hr = _AudioClient->Start();
		if (FAILED(hr))
		{
			printf("Unable to get new capture client: %x.\n", hr);
			return false;
		}

		bool stillPlaying = true;
		//while (stillPlaying)
		//{
		//	DWORD waitResult = WaitForSingleObject(_AudioSamplesReadyEvent, INFINITE);

		//	BYTE* pData, * pBuffer;
		//	INT nBufferLenght;
		//	UINT32 framesAvailable;
		//	DWORD  flags;
		//	pBuffer = new BYTE[MAX_AUDIO_FRAME_SIZE];
		//	hr = _CaptureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
		//	if (SUCCEEDED(hr))
		//	{
		//		if (framesAvailable != 0)
		//		{
		//			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
		//			{
		//				//
		//				//  Fill 0s from the capture buffer to the output buffer.
		//				//
		//			}
		//			else
		//			{
		//				//
		//				//  Copy data from the audio engine buffer to the output buffer.
		//				//

		//				CopyMemory(pBuffer, pData, framesAvailable * _FrameSize);
		//				printf("get capture frames: %d!\n", framesAvailable);
		//			}
		//		}
		//		delete[] pBuffer;
		//		hr = _CaptureClient->ReleaseBuffer(framesAvailable);
		//		if (FAILED(hr))
		//		{
		//			printf("Unable to release capture buffer: %x!\n", hr);
		//		}
		//	}
		//}
		m->m_port = Measure::PORT_OUTPUT;
		FILE* fi = NULL;
		fopen_s(&fi, "RosyAudioLevel.cfg", "r");
		int a = 4096, b = 2048, c = 100, d = 16500, e = 40, f = 30;
		fscanf_s(fi, "%d %d %d %d %d %d", &a, &b, &c, &d, &e, &f);

		m->m_fftSize = a;
		m->m_fftOverlap = b;
		m->m_freqMin = c;
		m->m_freqMax = d;
		m->m_sensitivity = e;
		m->m_nBands = f;
		m->m_enum = deviceEnumerator;
		m->m_clAudio = _AudioClient;
		m->m_wfx = _MixFormat;
		m->m_clCapture = _CaptureClient;
		m->m_channel = m->CHANNEL_FR;
		switch (_MixFormat->wFormatTag) {
		case WAVE_FORMAT_PCM:
			if (m->m_wfx->wBitsPerSample == 16) {
				m->m_format = m->FMT_PCM_S16;
			}
			break;
		case WAVE_FORMAT_IEEE_FLOAT:
			m->m_format = m->FMT_PCM_F32;
			break;
		case WAVE_FORMAT_EXTENSIBLE:
			if (reinterpret_cast<WAVEFORMATEXTENSIBLE*>(_MixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
				m->m_format = m->FMT_PCM_F32;
			}
			break;
		}
		if (m->m_fftSize) {
			for (int iChan = 0; iChan < m->m_wfx->nChannels; ++iChan) {
				m->m_fftCfg[iChan] = kiss_fftr_alloc(m->m_fftSize, 0, NULL, NULL);
				m->m_fftIn[iChan] = (float*)calloc(m->m_fftSize * sizeof(float), 1);
				m->m_fftOut[iChan] = (float*)calloc(m->m_fftSize * sizeof(float), 1);
			}
			m->m_fftKWdw = (float*)calloc(m->m_fftSize * sizeof(float), 1);
			m->m_fftTmpIn = (float*)calloc(m->m_fftSize * sizeof(float), 1);
			m->m_fftTmpOut = (kiss_fft_cpx*)calloc(m->m_fftSize * sizeof(kiss_fft_cpx), 1);
			m->m_fftBufP = m->m_fftSize - m->m_fftOverlap;
			// calculate window function coefficients (http://en.wikipedia.org/wiki/Window_function#Hann_.28Hanning.29_window)
			for (int iBin = 0; iBin < m->m_fftSize; ++iBin) {
				m->m_fftKWdw[iBin] = (float)(0.5 * (1.0 - cos(TWOPI * iBin / (m->m_fftSize - 1))));
			}
		}
		// calculate band frequencies and allocate band output buffers
		if (m->m_nBands) {
			m->m_bandFreq = (float*)malloc(m->m_nBands * sizeof(float));
			const double step = (log(m->m_freqMax / m->m_freqMin) / m->m_nBands) / log(2.0);
			m->m_bandFreq[0] = (float)(m->m_freqMin * pow(2.0, step / 2.0));
			for (int iBand = 1; iBand < m->m_nBands; ++iBand) {
				m->m_bandFreq[iBand] = (float)(m->m_bandFreq[iBand - 1] * pow(2.0, step));
			}
			for (int iChan = 0; iChan < m->m_wfx->nChannels; ++iChan) {
				m->m_bandOut[iChan] = (float*)calloc(m->m_nBands * sizeof(float), 1);
			}
		}
	}
	return true;
Exit:
	return false;
}

wstring CaptureAudioData(int nFlag) {

	Measure::Channel Chn = Measure::CHANNEL_SUM;
	switch (nFlag)
	{
	case 1:Chn = Measure::CHANNEL_FL;
		break;
	case 2:Chn = Measure::CHANNEL_FR;
		break;
	default:
		break;
	}
	wstring DataStr = L"[]";

	HRESULT hr;
	Measure* parent = (m->m_parent) ? m->m_parent : m;
	LARGE_INTEGER	pcCur;
	QueryPerformanceCounter(&pcCur);
	if (
		m->m_clCapture
		&& (((pcCur.QuadPart - m->m_pcPoll.QuadPart) * m->m_pcMult) >= QUERY_TIMEOUT)
		) {
		while ((hr = m->m_clCapture->GetBuffer(&buffer, &nFrames, &flags, &pos, NULL)) == S_OK) {
			// measure RMS and peak levels
			float		rms[Measure::MAX_CHANNELS];
			float		peak[Measure::MAX_CHANNELS];
			for (int iChan = 0; iChan < Measure::MAX_CHANNELS; ++iChan) {
				rms[iChan] = (float)m->m_rms[iChan];
				peak[iChan] = (float)m->m_peak[iChan];
			}
			// loops unrolled for float, 16b and mono, stereo
			if (m->m_format == Measure::FMT_PCM_F32) {
				float* s = (float*)buffer;
				if (m->m_wfx->nChannels == 1) {
					for (unsigned int iFrame = 0; iFrame < nFrames; ++iFrame) {
						float xL = (float)*s++;
						float sqrL = xL * xL;
						float absL = abs(xL);
						rms[0] = sqrL + m->m_kRMS[(sqrL < rms[0])] * (rms[0] - sqrL);
						peak[0] = absL + m->m_kPeak[(absL < peak[0])] * (peak[0] - absL);
						rms[1] = rms[0];
						peak[1] = peak[0];
					}
				}
				else if (m->m_wfx->nChannels == 2) {
					for (unsigned int iFrame = 0; iFrame < nFrames; ++iFrame) {
						float xL = (float)*s++;
						float xR = (float)*s++;
						float sqrL = xL * xL;
						float sqrR = xR * xR;
						float absL = abs(xL);
						float absR = abs(xR);
						rms[0] = sqrL + m->m_kRMS[(sqrL < rms[0])] * (rms[0] - sqrL);
						rms[1] = sqrR + m->m_kRMS[(sqrR < rms[1])] * (rms[1] - sqrR);
						peak[0] = absL + m->m_kPeak[(absL < peak[0])] * (peak[0] - absL);
						peak[1] = absR + m->m_kPeak[(absR < peak[1])] * (peak[1] - absR);
					}
				}
				else {
					for (unsigned int iFrame = 0; iFrame < nFrames; ++iFrame) {
						for (unsigned int iChan = 0; iChan < m->m_wfx->nChannels; ++iChan) {
							float	x = (float)*s++;
							float	sqrX = x * x;
							float	absX = abs(x);
							rms[iChan] = sqrX + m->m_kRMS[(sqrX < rms[iChan])] * (rms[iChan] - sqrX);
							peak[iChan] = absX + m->m_kPeak[(absX < peak[iChan])] * (peak[iChan] - absX);
						}
					}
				}
			}
			else if (m->m_format == Measure::FMT_PCM_S16) {
				INT16* s = (INT16*)buffer;
				if (m->m_wfx->nChannels == 1) {
					for (unsigned int iFrame = 0; iFrame < nFrames; ++iFrame) {
						float xL = (float)*s++ * 1.0f / 0x7fff;
						float sqrL = xL * xL;
						float absL = abs(xL);
						rms[0] = sqrL + m->m_kRMS[(sqrL < rms[0])] * (rms[0] - sqrL);
						peak[0] = absL + m->m_kPeak[(absL < peak[0])] * (peak[0] - absL);
						rms[1] = rms[0];
						peak[1] = peak[0];
					}
				}
				else if (m->m_wfx->nChannels == 2) {
					for (unsigned int iFrame = 0; iFrame < nFrames; ++iFrame) {
						float xL = (float)*s++ * 1.0f / 0x7fff;
						float xR = (float)*s++ * 1.0f / 0x7fff;
						float sqrL = xL * xL;
						float sqrR = xR * xR;
						float absL = abs(xL);
						float absR = abs(xR);
						rms[0] = sqrL + m->m_kRMS[(sqrL < rms[0])] * (rms[0] - sqrL);
						rms[1] = sqrR + m->m_kRMS[(sqrR < rms[1])] * (rms[1] - sqrR);
						peak[0] = absL + m->m_kPeak[(absL < peak[0])] * (peak[0] - absL);
						peak[1] = absR + m->m_kPeak[(absR < peak[1])] * (peak[1] - absR);
					}
				}
				else {
					for (unsigned int iFrame = 0; iFrame < nFrames; ++iFrame) {
						for (unsigned int iChan = 0; iChan < m->m_wfx->nChannels; ++iChan) {
							float	x = (float)*s++ * 1.0f / 0x7fff;
							float	sqrX = x * x;
							float	absX = abs(x);
							rms[iChan] = sqrX + m->m_kRMS[(sqrX < rms[iChan])] * (rms[iChan] - sqrX);
							peak[iChan] = absX + m->m_kPeak[(absX < peak[iChan])] * (peak[iChan] - absX);
						}
					}
				}
			}
			for (int iChan = 0; iChan < Measure::MAX_CHANNELS; ++iChan) {
				m->m_rms[iChan] = rms[iChan];
				m->m_peak[iChan] = peak[iChan];
			}

			// process FFTs (optional)
			if (m->m_fftSize) {
				float* sF32 = (float*)buffer;
				INT16* sI16 = (INT16*)buffer;
				const float	scalar = (float)(1.0 / sqrt(m->m_fftSize));
				for (unsigned int iFrame = 0; iFrame < nFrames; ++iFrame) {
					// fill ring buffers (demux streams)
					for (unsigned int iChan = 0; iChan < m->m_wfx->nChannels; ++iChan) {
						(m->m_fftIn[iChan])[m->m_fftBufW] = (m->m_format == Measure::FMT_PCM_F32) ? *sF32++ : ((float)*sI16++ * 1.0f / 0x7fff);
					}
					m->m_fftBufW = (m->m_fftBufW + 1) % m->m_fftSize;
					// if overlap limit reached, process FFTs for each channel
					if (!--m->m_fftBufP) {
						for (unsigned int iChan = 0; iChan < m->m_wfx->nChannels; ++iChan) {
							if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
								// copy from the ring buffer to temp space
								memcpy(&m->m_fftTmpIn[0], &(m->m_fftIn[iChan])[m->m_fftBufW], (m->m_fftSize - m->m_fftBufW) * sizeof(float));
								memcpy(&m->m_fftTmpIn[m->m_fftSize - m->m_fftBufW], &m->m_fftIn[iChan][0], m->m_fftBufW * sizeof(float));
								// apply the windowing function
								for (int iBin = 0; iBin < m->m_fftSize; ++iBin) {
									m->m_fftTmpIn[iBin] *= m->m_fftKWdw[iBin];
								}
								kiss_fftr(m->m_fftCfg[iChan], m->m_fftTmpIn, m->m_fftTmpOut);
							}
							else {
								memset(m->m_fftTmpOut, 0, m->m_fftSize * sizeof(kiss_fft_cpx));
							}
							// filter the bin levels as with peak measurements
							for (int iBin = 0; iBin < m->m_fftSize; ++iBin) {
								float x0 = (m->m_fftOut[iChan])[iBin];
								float x1 = (m->m_fftTmpOut[iBin].r * m->m_fftTmpOut[iBin].r + m->m_fftTmpOut[iBin].i * m->m_fftTmpOut[iBin].i) * scalar;
								x0 = x1 + m->m_kFFT[(x1 < x0)] * (x0 - x1);
								(m->m_fftOut[iChan])[iBin] = x0;
							}
						}
						m->m_fftBufP = m->m_fftSize - m->m_fftOverlap;
					}
				}
				// integrate FFT results into log-scale frequency bands
				if (m->m_nBands) {
					const float		df = (float)m->m_wfx->nSamplesPerSec / m->m_fftSize;
					const float		scalar = 2.0f / (float)m->m_wfx->nSamplesPerSec;
					for (unsigned int iChan = 0; iChan < m->m_wfx->nChannels; ++iChan) {
						memset(m->m_bandOut[iChan], 0, m->m_nBands * sizeof(float));
						int			iBin = 0;
						int			iBand = 0;
						float		f0 = 0.0f;
						while (
							(iBin <= m->m_fftSize / 2)
							&& (iBand < m->m_nBands)
							) {
							float	fLin1 = ((float)iBin + 0.5f) * df;
							float	fLog1 = m->m_bandFreq[iBand];
							float	x = (m->m_fftOut[iChan])[iBin];
							float& y = (m->m_bandOut[iChan])[iBand];
							if (fLin1 <= fLog1) {
								y += (fLin1 - f0) * x * scalar;
								f0 = fLin1;
								iBin += 1;
							}
							else {
								y += (fLog1 - f0) * x * scalar;
								f0 = fLog1;
								iBand += 1;
							}
						}
					}
				}
			}

			// release the buffer
			m->m_clCapture->ReleaseBuffer(nFrames);

			// mark the time of last buffer update
			m->m_pcFill = pcCur;
		}
		// detect device disconnection
		switch (hr) {
		case AUDCLNT_S_BUFFER_EMPTY:
			// Windows bug: sometimes when shutting down a playback application, it doesn't zero
			// out the buffer.  Detect this by checking the time since the last successful fill
			// and resetting the volumes if past the threshold.
			if (((pcCur.QuadPart - m->m_pcFill.QuadPart) * m->m_pcMult) >= EMPTY_TIMEOUT) {
				for (int iChan = 0; iChan < Measure::MAX_CHANNELS; ++iChan) {
					m->m_rms[iChan] = 0.0;
					m->m_peak[iChan] = 0.0;
				}
			}
			break;
		case AUDCLNT_E_BUFFER_ERROR:
		case AUDCLNT_E_DEVICE_INVALIDATED:
		case AUDCLNT_E_SERVICE_NOT_RUNNING:

			break;
		}
		m->m_pcPoll = pcCur;
	}
	else if (
		!m->m_parent
		&& !m->m_clCapture
		&& (((pcCur.QuadPart - m->m_pcPoll.QuadPart) * m->m_pcMult) >= DEVICE_TIMEOUT)
		) {
		// poll for new devices
		assert(m->m_enum);
		assert(!m->m_dev);
		m->m_pcPoll = pcCur;
	}
	if (parent->m_clCapture && parent->m_nBands) {
		DataStr = L"[";
		for (int i = 0; i < m->m_nBands; i++) {
			double	x;
			const int iBand = /*m->m_bandIdx*/i;
			if (Chn == Measure::CHANNEL_SUM) {
				if (parent->m_wfx->nChannels >= 2) {
					x = (parent->m_bandOut[0][iBand] + parent->m_bandOut[1][iBand]) * 0.5;
				}
				else {
					x = parent->m_bandOut[0][iBand];
				}
			}
			else if (Chn < parent->m_wfx->nChannels) {
				x = parent->m_bandOut[Chn][iBand];
			}
			x = CLAMP01(x);
			x = max(0, 10.0 / parent->m_sensitivity * log10(x) + 1.0);
			DataStr += to_wstring(x);
			if (i < m->m_nBands - 1) {
				DataStr += L",";
			}
		}
		DataStr += L"]";
	}
	return DataStr;
}