#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

#pragma pack(push)
#pragma pack(1)

#define SAMPLES_PER_SECOND 44100
#define BITS_PER_SAMPLE 24

extern const char* FCC_RIFF;
extern const char* FCC_WAVE;
extern const char* FCC_FMT;
extern const char* FCC_DATA;
extern const char* FCC_FACT;
extern const char* FCC_WAVL;
extern const char* FCC_SLNT;
extern const char* FCC_BEXT;
extern const char* FCC_CUE;
extern const char* FCC_PLST;
extern const char* FCC_LIST;
extern const char* FCC_LABL;
extern const char* FCC_NOTE;
extern const char* FCC_LTXT;
extern const char* FCC_SMPL;
extern const char* FCC_INST;
extern const char* FCC_ADTL;
extern const char* FCC_JUNK;

union FourLetterCode
{
	unsigned int uCode;
	char acText[4];

	FourLetterCode()
	{
		uCode = 0;
	}

	FourLetterCode(unsigned int oneNumber)
	{
		*this = oneNumber;
	}

	FourLetterCode(const char* fourLetters)
	{
		*this = fourLetters;
	}

	bool operator ==(const FourLetterCode& other)
	{
		return other.uCode == this->uCode;
	}

	bool operator !=(const FourLetterCode& other)
	{
		return other.uCode != this->uCode;
	}

	bool Equals(const char* fourLetters)
	{
		return *(unsigned int*)fourLetters == this->uCode;
	}

	operator const char*()
	{
		return acText;
	}

	operator unsigned int()
	{
		return uCode;
	}

	FourLetterCode& operator=(const char* fourLetters)
	{
		this->uCode = *(unsigned int*)fourLetters;
		return *this;
	}

	FourLetterCode& operator=(unsigned int oneNumber)
	{
		this->uCode = oneNumber;
		return *this;
	}
};

struct ChunkHeader
{
	FourLetterCode code;
	unsigned int uChunkSize;

	ChunkHeader()
	{
		code = 0u;
		uChunkSize = 0;
	}

	ChunkHeader(const char* fourLetterCode, unsigned int uSize)
	{
		Init(fourLetterCode, uSize);
	}

	void Init(const char* fourLetterCode, unsigned int uSize)
	{
		code = fourLetterCode;
		uChunkSize = uSize;
	}

	bool IsTypeOf(const char* fourLetterCode)
	{
		return code == FourLetterCode(fourLetterCode);
	}
};

struct RIFFChunk : public ChunkHeader
{
	FourLetterCode wave;

	RIFFChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_RIFF, 0);
		wave = FCC_WAVE;
	}

	bool IsValid()
	{
		return 
			IsTypeOf(FCC_RIFF) && 0 == uChunkSize
			&& wave.Equals(FCC_WAVE);
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(RIFFChunk) - sizeof(ChunkHeader);
	}
};

struct FormatChunk : public ChunkHeader
{
	unsigned short wWaveFormat;				// Format code (enum WaveFormatType)

private:
	unsigned short wChannels;				// Number of interleaved channels
	unsigned int uSamplesPerSec;			// Sampling rate (blocks per second)
	unsigned int uAvgBytesPerSec;			// Data rate
	unsigned short wBlockAlign;				// Data block size (bytes)
	unsigned short wBitsPerSample;			// Bits per sample

public:
	FormatChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_FMT, GetChunkSize());
		wWaveFormat = (unsigned short)WAVE_FORMAT_PCM;
		wChannels = 1;
		uSamplesPerSec = SAMPLES_PER_SECOND;
		wBitsPerSample = BITS_PER_SAMPLE;

		Update();
	}

	bool IsValid()
	{
		return 
			IsTypeOf(FCC_FMT) && uChunkSize == GetChunkSize()
			&& wWaveFormat == (unsigned short)WAVE_FORMAT_PCM
			&& wChannels == 1
			&& uSamplesPerSec == SAMPLES_PER_SECOND
			&& wBitsPerSample == BITS_PER_SAMPLE;
	}

	void SetChannels(unsigned short value)
	{
		wChannels = value;
		Update();
	}

	unsigned short GetChannels()
	{
		return wChannels;
	}

	void SetBitsPerSample(unsigned short value)
	{
		wBitsPerSample = value;
		Update();
	}

	unsigned short GetBitsPerSample()
	{
		return wBitsPerSample;
	}

	void SetSamplesPerSec(unsigned int value)
	{
		uSamplesPerSec = value;
		Update();
	}

	unsigned int GetSamplesPerSec()
	{
		return uSamplesPerSec;
	}

	void SetBlockAlign(unsigned short value)
	{
		wBlockAlign = value;
		Update();
	}

	unsigned short GetBlockAlign()
	{
		return wBlockAlign;
	}

	static unsigned int GetChunkSize()
	{
		return sizeof(FormatChunk) - sizeof(ChunkHeader);
	}

private:
	void Update()
	{
		wBlockAlign = wChannels * wBitsPerSample / 8;
		uAvgBytesPerSec = uSamplesPerSec * wBlockAlign;
	}
};

struct FormatChunkExt1 : public FormatChunk
{
	unsigned short wSize;					// Size of the extension (0 or 22)

	FormatChunkExt1()
	{
		Init();
	}

	void Init()
	{
		FormatChunk::Init();
		uChunkSize = GetChunkSize();
		wSize = 0;
	}

	static unsigned int  GetChunkSize()
	{
		return sizeof(FormatChunkExt1) - sizeof(ChunkHeader);
	}

};

struct FormatChunkExt2 : public FormatChunkExt1
{
	unsigned short wValidBitsPerSample;		// Number of valid bits
	unsigned int dwChannelMask;				// Speaker position mask
	char acSubFormat[16];					// GUID, including the data format code

	FormatChunkExt2()
	{
		Init();
	}

	void Init()
	{
		FormatChunkExt1::Init();
		uChunkSize = GetChunkSize();
		wValidBitsPerSample = 0;
		dwChannelMask = 0;
		ZeroMemory(acSubFormat, sizeof(acSubFormat));
	}

	static unsigned int  GetChunkSize()
	{
		return sizeof(FormatChunkExt2) - sizeof(ChunkHeader);
	}

};

struct DataChunk : public ChunkHeader
{
	DataChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_DATA, 0);
	}

	bool IsValid()
	{
		return IsTypeOf(FCC_DATA);
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(DataChunk) - sizeof(ChunkHeader);
	}
};

struct FactChunk : public ChunkHeader
{
	unsigned int uSamples;

	FactChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_FACT, GetChunkSize());
		uSamples = 0;
	}

	unsigned int GetChunkSize()
	{
		return sizeof(FactChunk) - sizeof(ChunkHeader);
	}
};

struct WaveListChunk : public ChunkHeader
{
	WaveListChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_WAVL, GetEmptyChunkSize());
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(WaveListChunk) - sizeof(ChunkHeader);
	}

};

struct SilentChunk : public ChunkHeader
{
	unsigned int uSilentSamples;

	SilentChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_SLNT, GetChunkSize());
	}

	unsigned int GetChunkSize()
	{
		return sizeof(SilentChunk) - sizeof(ChunkHeader);
	}
};

struct BroadcastAudioExtensionChunk : public ChunkHeader
{
	char acDescription[256];				// ASCII : «Description of the sound sequence»
	char acOriginator[32];					// ASCII : «Name of the originator» */
	char acOriginatorReference[32];			// ASCII : «Reference of the originator» */
	char acOriginationDate[10];				// ASCII : «yyyy:mm:dd» */
	char acOriginationTime[8];				// ASCII : «hh:mm:ss» */
	unsigned int uTimeReferenceLow;			// First sample count since midnight, low unsigned short	*/
	unsigned int uTimeReferenceHigh;		// First sample count since midnight, high unsigned short	*/
	unsigned short wVersion;				// Version of the BWF; unsigned binary number */
	unsigned char UMID[64];					// Binary unsigned char 0 to 63 of SMPTE UMID */
	unsigned short wLoudnessValue;			// unsigned short : «Integrated Loudness Value of the file in LUFS (multiplied by 100) » */
	unsigned short wLoudnessRange;			// unsigned short : «Loudness Range of the file in LU (multiplied by 100) » */
	unsigned short wMaxTruePeakLevel;		// unsigned short : «Maximum True Peak Level of the file expressed as dBTP (multiplied by 100) » */
	unsigned short wMaxMomentaryLoudness;	// unsigned short : «Highest value of the Momentary Loudness Level of the file in LUFS (multiplied by 100) » */
	unsigned short wMaxShortTermLoudness;	// unsigned short : «Highest value of the Short-Term Loudness Level of the file in LUFS (multiplied	by 100) » */
	unsigned char acReserved[180];			// 180 bytes, reserved for future use, set to “NULL” */
	//	char acCodingHistory[];					// ASCII : « History coding » */

	BroadcastAudioExtensionChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_BEXT, GetChunkSize());
		ZeroMemory(acDescription, sizeof(acDescription));
		ZeroMemory(acOriginator, sizeof(acOriginator));
		ZeroMemory(acOriginatorReference, sizeof(acOriginatorReference));
		ZeroMemory(acOriginationDate, sizeof(acOriginationDate));
		ZeroMemory(acOriginationTime, sizeof(acOriginationTime));
		uTimeReferenceLow = 0;
		uTimeReferenceHigh = 0;
		wVersion = 0;
		ZeroMemory(UMID, sizeof(UMID));
		wLoudnessValue = 0;
		wLoudnessRange = 0;
		wMaxTruePeakLevel = 0;
		wMaxMomentaryLoudness = 0;
		wMaxShortTermLoudness = 0;
		ZeroMemory(acReserved, sizeof(acReserved));
	}

	unsigned int GetChunkSize()
	{
		return sizeof(BroadcastAudioExtensionChunk) - sizeof(ChunkHeader);
	}
};

struct CueChunk : public ChunkHeader
{
	unsigned int uCuePoints;

	CueChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_CUE, GetEmptyChunkSize());
		uCuePoints = 0;
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(CueChunk) - sizeof(ChunkHeader);
	}
};

struct PlayListChunk : public ChunkHeader
{
	unsigned int uSegments;

	PlayListChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_PLST, GetEmptyChunkSize());
		uSegments = 0;
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(PlayListChunk) - sizeof(ChunkHeader);
	}

};

struct AssociatedDataListChunk : public ChunkHeader
{
	FourLetterCode typeId;

	AssociatedDataListChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_LIST, GetEmptyChunkSize());
		typeId = FCC_ADTL;
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(AssociatedDataListChunk) - sizeof(ChunkHeader);
	}

};

struct LabelChunk : public ChunkHeader
{
	unsigned int uCuePointId;

	LabelChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_LABL, GetEmptyChunkSize());
		uCuePointId = 0;
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(LabelChunk) - sizeof(ChunkHeader);
	}

};

struct NoteChunk : public ChunkHeader
{
	unsigned int uCuePointId;

	NoteChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_NOTE, GetEmptyChunkSize());
		uCuePointId = 0;
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(NoteChunk) - sizeof(ChunkHeader);
	}

};

struct LabeledTextChunk : public ChunkHeader
{
	unsigned int uCuePointId;
	unsigned int uSampleLength;
	unsigned int uPurposeID;
	unsigned short wCountry;
	unsigned short wLanguage;
	unsigned short wDialect;
	unsigned short wCodePage;

	LabeledTextChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_LTXT, GetEmptyChunkSize());
		uCuePointId = 0;
		uSampleLength = 0;
		uPurposeID = 0;
		wCountry = 0;
		wLanguage = 0;
		wDialect = 0;
		wCodePage = 0;
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(LabeledTextChunk) - sizeof(ChunkHeader);
	}

};

struct SamplerChunk : public ChunkHeader
{
	unsigned int Manufacturer;
	unsigned int Product;
	unsigned int SamplePeriod;
	unsigned int MIDIUnityNote;
	unsigned int MIDIPitchFraction;
	unsigned int SMPTEFormat;
	unsigned int SMPTEOffset;
	unsigned int NumSampleLoops;
	unsigned int SamplerData;

	SamplerChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_SMPL, GetEmptyChunkSize());
		Manufacturer = 0;
		Product = 0;
		SamplePeriod = 0;
		MIDIUnityNote = 0;
		MIDIPitchFraction = 0;
		SMPTEFormat = 0;
		SMPTEOffset = 0;
		NumSampleLoops = 0;
		SamplerData = 0;
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(SamplerChunk) - sizeof(ChunkHeader);
	}

};

struct InstrumentChunk : public ChunkHeader
{
	unsigned char bUnshiftedNote;
	char cFineTune;
	char cGain;
	unsigned char bLowNote;
	unsigned char bHighNote;
	unsigned char bLowVelocity;
	unsigned char bHighVelocity;

	InstrumentChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_INST, GetChunkSize());
		bUnshiftedNote = 0;
		cFineTune = 0;
		cGain = 0;
		bLowNote = 0;
		bHighNote = 0;
		bLowVelocity = 0;
		bHighVelocity = 0;
	}

	unsigned int GetChunkSize()
	{
		return sizeof(InstrumentChunk) - sizeof(ChunkHeader);
	}
};

struct JunkChunk : public ChunkHeader
{
	JunkChunk()
	{
		Init();
	}

	void Init()
	{
		ChunkHeader::Init(FCC_JUNK, GetEmptyChunkSize());
	}

	unsigned int GetEmptyChunkSize()
	{
		return sizeof(JunkChunk) - sizeof(ChunkHeader);
	}
};

struct FormatWaveHeader
{
	RIFFChunk	riffChunk;
	FormatChunk	formatChunk;

	void Init()
	{
		riffChunk.Init();
		formatChunk.Init();
	}

	bool IsValid()
	{
		return riffChunk.IsValid() && formatChunk.IsValid();
	}
};

struct FormatExt1WaveHeader
{
	RIFFChunk riffChunk;
	FormatChunkExt1 formatChunkExt1;

	void Init()
	{
		riffChunk.Init();
		formatChunkExt1.Init();
	}

	bool IsValid()
	{
		return riffChunk.IsValid() && formatChunkExt1.IsValid();
	}
};

struct FormatExt2WaveHeader
{
	RIFFChunk riffChunk;
	FormatChunkExt2 formatChunkExt2;

	void Init()
	{
		riffChunk.Init();
		formatChunkExt2.Init();
	}

	bool IsValid()
	{
		return riffChunk.IsValid() && formatChunkExt2.IsValid();
	}
};


#pragma pack(pop)


namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			/// <summary>
			/// Binary format of sample values
			/// </summary>
			public enum class SampleFormat : unsigned short
			{
				/// <summary>
				/// 16 or 32 bit integer
				/// </summary>
				PCM = 1,
				/// <summary>
				/// 32 bit floating point
				/// </summary>
				IEEEFloat = 3,
				/// <summary>
				/// MuLaw?
				/// </summary>
				MuLaw = 7,
				/// <summary>
				/// Extensible?
				/// </summary>
				Extensible = 0xFFFEu
			};

			/// <summary>
			/// Format information header of wave file
			/// </summary>
			public ref class WaveFormat
			{
			public:
				/// <summary>
				/// Constructor
				/// </summary>
				WaveFormat();

				/// <summary>
				/// Writes the header chunks to the given <paramref name="wavStream"/>
				/// </summary>
				/// <param name="wavStream">Stream to write to</param>
				/// <remarks>
				/// When a <see cref="WaveFile"/> was opened in Record mode, <see cref="WaveFile.Close()"/> calls this
				/// after calculating the sample count according to the recorded data size.
				/// </remarks>
				void WriteHeaderChunks(System::IO::Stream^ wavStream);

				/// <summary>
				/// Reads the header chunks of the given <paramref name="wavStream"/>
				/// </summary>
				/// <param name="wavStream">Stream to read from</param>
				void ReadToData(System::IO::Stream^ wavStream);

				/// <summary>
				/// Gets or sets the number of audio channels.
				/// </summary>
				property int Channels;

				/// <summary>
				/// Gets or sets the total number of samples.
				/// </summary>
				/// <remarks>
				/// When writing a <see cref="WaveFile"/>, this is automatically updated by <see cref="WaveFile.Close()"/>
				/// </remarks>
				property int SampleCount;

				/// <summary>
				/// Gets or sets the size of a single sample in bytes.
				/// </summary>
				property int SampleSize;

				/// <summary>
				/// Gets the binary sample format.
				/// </summary>
				/// <seealso cref="Audio.Asio.SampleFormat"/>
				property Audio::Foundation::Interop::SampleFormat SampleFormat;

			private:
				int ReadNextChunk(System::IO::Stream^ wavStream, unsigned char* pcBuffer, int iMax);
				int SkipChunkContent(System::IO::Stream^ wavStream, ChunkHeader* pChunkHeader, int iSizeRead);
				int GetMaximumChunkSize();
			};
		}
	}
}