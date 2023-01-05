#include "pch.h"
#include "WaveFormat.h"

using namespace Audio::Foundation::Interop;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::IO;

const char* FCC_RIFF = "RIFF";
const char* FCC_WAVE = "WAVE";
const char* FCC_FMT  = "fmt ";
const char* FCC_DATA = "data";
const char* FCC_FACT = "fact";
const char* FCC_WAVL = "wavl";
const char* FCC_SLNT = "slnt";
const char* FCC_BEXT = "bext";
const char* FCC_CUE  = "cue ";
const char* FCC_PLST = "plst";
const char* FCC_LIST = "list";
const char* FCC_LABL = "labl";
const char* FCC_NOTE = "note";
const char* FCC_LTXT = "ltxt";
const char* FCC_SMPL = "smpl";
const char* FCC_INST = "inst";
const char* FCC_ADTL = "adtl";
const char* FCC_JUNK = "junk";

WaveFormat::WaveFormat()
{
}

void WaveFormat::WriteHeaderChunks(Stream^ wavStream)
{
	unsigned int uRiffAndFormatChunkSize = 0;
	bool hasFactChunk = false;

	switch(SampleFormat)
	{
	case Audio::Foundation::Interop::SampleFormat::PCM:
		uRiffAndFormatChunkSize = sizeof(FormatWaveHeader);
		hasFactChunk = false;
		break;

	case Audio::Foundation::Interop::SampleFormat::Extensible:
		uRiffAndFormatChunkSize = sizeof(FormatExt2WaveHeader);
		hasFactChunk = true;
		break;

	default:
		uRiffAndFormatChunkSize = sizeof(FormatExt1WaveHeader);
		hasFactChunk = true;
		break;
	}

	unsigned int uDataChunkOffset = uRiffAndFormatChunkSize + (hasFactChunk? sizeof(FactChunk) : 0); 
	unsigned int uWavHeaderSize =  uDataChunkOffset + sizeof(DataChunk);
	unsigned int uDataSize = SampleCount * SampleSize * Channels;
	unsigned int uFileSize = uWavHeaderSize + uDataSize;

	FactChunk factChunk;
	factChunk.uSamples = SampleCount * Channels;

	DataChunk dataChunk;
	dataChunk.uChunkSize = dataChunk.GetEmptyChunkSize() + uDataSize;

	FormatExt2WaveHeader header;

	header.formatChunkExt2.uChunkSize = uRiffAndFormatChunkSize - sizeof(RIFFChunk) - sizeof(ChunkHeader);
	header.formatChunkExt2.wWaveFormat = (unsigned short)SampleFormat;
	header.formatChunkExt2.SetBitsPerSample(SampleSize * 8);
	header.formatChunkExt2.SetChannels(Channels);
	header.formatChunkExt2.SetSamplesPerSec(SampleRate);
	header.riffChunk.uChunkSize = uFileSize - sizeof(ChunkHeader); 
	array<Byte,1>^ buffer = gcnew array<Byte,1>(uWavHeaderSize);

	Marshal::Copy(IntPtr((void*)&header), buffer, 0, uRiffAndFormatChunkSize); 

	if(hasFactChunk)
		Marshal::Copy(IntPtr((void*)&factChunk), buffer, uRiffAndFormatChunkSize, sizeof(FactChunk));

	Marshal::Copy(IntPtr((void*)&dataChunk), buffer, uDataChunkOffset, sizeof(DataChunk));

	wavStream->Write(buffer, 0, uWavHeaderSize);
}

void WaveFormat::ReadToData(Stream^ wavStream)
{
	unsigned char* pChunkBuffer = NULL;

	try
	{
		int iMaximumChunkSize = GetMaximumChunkSize();
		pChunkBuffer = new unsigned char[iMaximumChunkSize];
		ZeroMemory(pChunkBuffer, iMaximumChunkSize);
		ChunkHeader* pChunkHeader = (ChunkHeader*)pChunkBuffer;
		int iRead = 0;

		iRead = ReadNextChunk(wavStream, pChunkBuffer, iMaximumChunkSize);
		if(!pChunkHeader->IsTypeOf(FCC_RIFF))
			throw gcnew FormatException("RIFF chunk not found");

		iRead = ReadNextChunk(wavStream, pChunkBuffer, iMaximumChunkSize);
		if(!pChunkHeader->IsTypeOf(FCC_FMT))
			throw gcnew FormatException("Format chunk not found");

		FormatChunk* pFormatChunk = (FormatChunk*)pChunkBuffer;

		if(0 == pFormatChunk->GetChannels())
			throw gcnew FormatException("Invalid format chunk: Number of channels must be greater than zero.");

		SampleFormat = (Audio::Foundation::Interop::SampleFormat)(pFormatChunk->wWaveFormat);
		SampleSize = pFormatChunk->GetBlockAlign() / pFormatChunk->GetChannels();

		if(0 == SampleSize)
			throw gcnew FormatException("Unexpected sample format");

		SampleRate = pFormatChunk->GetSamplesPerSec();
		Channels = pFormatChunk->GetChannels();

		while(iRead = ReadNextChunk(wavStream, pChunkBuffer, iMaximumChunkSize))
		{
			if(pChunkHeader->IsTypeOf(FCC_FACT))
			{
				FactChunk* pFactChunk = (FactChunk*)pChunkBuffer;
				SampleCount = pFactChunk->uSamples;
			}
			else if(pChunkHeader->IsTypeOf(FCC_DATA))
			{
				DataChunk* pDataChunk = (DataChunk*)pChunkBuffer;
				SampleCount = pDataChunk->uChunkSize / SampleSize;
				break;
			}
			else 
				SkipChunkContent(wavStream, pChunkHeader, iRead); 
		}
	}
	catch(IOException^ iox)
	{
		throw gcnew FormatException("Error reading WAV headers", iox);
	}
	finally
	{
		if(NULL != pChunkBuffer)
			delete[] pChunkBuffer;
	}
}

int WaveFormat::GetMaximumChunkSize()
{
	int iMaxChunkSize = max(sizeof(RIFFChunk), sizeof(DataChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(FactChunk)); 
	iMaxChunkSize = max(iMaxChunkSize, sizeof(FormatChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(FormatChunkExt1));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(FormatChunkExt2)); 
	iMaxChunkSize = max(iMaxChunkSize, sizeof(WaveListChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(SilentChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(BroadcastAudioExtensionChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(CueChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(PlayListChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(AssociatedDataListChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(LabelChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(NoteChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(LabeledTextChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(SamplerChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(InstrumentChunk));
	iMaxChunkSize = max(iMaxChunkSize, sizeof(JunkChunk));

	return iMaxChunkSize;
}

int WaveFormat::SkipChunkContent(Stream^ wavStream, ChunkHeader* pChunkHeader, int iReadSize)
{
	int iRemain = pChunkHeader->uChunkSize + sizeof(ChunkHeader) - iReadSize;

	if(iRemain > 0)
		wavStream->Seek(iRemain, SeekOrigin::Current);

	return iRemain;
}

int WaveFormat::ReadNextChunk(Stream^ wavStream, unsigned char* pcBuffer, int iMaxChunkSize)
{
	if(iMaxChunkSize < sizeof(ChunkHeader))
		throw gcnew ArgumentException(String::Format("Destination buffer must be at least {0} bytes.", sizeof(ChunkHeader)));

	ChunkHeader header;
	array<unsigned char>^ chunkBytes = gcnew array<unsigned char>(iMaxChunkSize);
	int iRead = wavStream->Read(chunkBytes, 0, sizeof(ChunkHeader));
	int iRemain = 0;

	ZeroMemory(pcBuffer, iMaxChunkSize);
	if(iRead != sizeof(ChunkHeader))
		throw gcnew FormatException("Unable to read next chunk header.");
	else
	{
		Marshal::Copy(chunkBytes, 0, IntPtr((void*)&header), iRead);

		if(header.IsTypeOf(FCC_RIFF))
			iRemain = sizeof(RIFFChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_FACT))
			iRemain = sizeof(FactChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_DATA))
			iRemain = sizeof(DataChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_FMT))
		{
			switch(header.uChunkSize + sizeof(ChunkHeader))
			{
			case sizeof(FormatChunk):
				iRemain = sizeof(FormatChunk) - sizeof(ChunkHeader);
				break;
			case sizeof(FormatChunkExt1):
				iRemain = sizeof(FormatChunkExt1) - sizeof(ChunkHeader);
				break;
			case sizeof(FormatChunkExt2):
				iRemain = sizeof(FormatChunkExt2) - sizeof(ChunkHeader);
				break;
			}
		}
		else if(header.IsTypeOf(FCC_BEXT))
			iRemain = sizeof(BroadcastAudioExtensionChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_WAVL))
			iRemain = sizeof(WaveListChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_SLNT))
			iRemain = sizeof(SilentChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_CUE))
			iRemain = sizeof(CueChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_PLST))
			iRemain = sizeof(PlayListChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_LIST))
			iRemain = sizeof(AssociatedDataListChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_LABL))
			iRemain = sizeof(LabelChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_NOTE))
			iRemain = sizeof(NoteChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_LTXT))
			iRemain = sizeof(LabeledTextChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_SMPL))
			iRemain = sizeof(SamplerChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_INST))
			iRemain = sizeof(InstrumentChunk) - sizeof(ChunkHeader);
		else if(header.IsTypeOf(FCC_JUNK))
			iRemain = sizeof(JunkChunk) - sizeof(ChunkHeader);
		else
		{
			String^ chunkCode = gcnew String(header.code, 0, sizeof(header.code));
			throw gcnew FormatException(String::Format("Unsupported chunk type \"{0}\".", chunkCode));
		}
		CopyMemory(pcBuffer, &header, sizeof(ChunkHeader));
		if(iRemain > 0)
		{
			if(iRead + iRemain > iMaxChunkSize)
				throw gcnew ArgumentException(String::Format(
				"Destination buffer too small. Available bytes: {0}. Required bytes: {1}.", iMaxChunkSize, iRead + iRemain));

			int iReadNext = wavStream->Read(chunkBytes, sizeof(ChunkHeader), iRemain);

			if(iReadNext != iRemain)
				throw gcnew FormatException(String::Format("Unable to read calculated chunk remainder of {0} byte(s).", iRemain));
			Marshal::Copy(chunkBytes, sizeof(ChunkHeader), IntPtr((void*)(pcBuffer + sizeof(ChunkHeader))), iRemain);
			iRead += iReadNext;
		}
	}
	return iRead;
}

