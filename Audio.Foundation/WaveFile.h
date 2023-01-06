#pragma once
#include "SampleConversion.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			ref class WaveFormat;

			/// <summary>
			/// A wave file (*.wav) for audio sample data
			/// </summary>
			public ref class WaveFile sealed : public System::IDisposable
			{
			public:
				/// <summary>
				/// WaveFile mode
				/// </summary>
				enum class Mode
				{
					/// <summary>
					/// WaveFile is closed
					/// </summary>
					Closed,
					/// <summary>
					/// WaveFile is in recording mode, e.g. data is written to the file.
					/// </summary>
					Record,
					/// <summary>
					/// WaveFile is in play mode, e.g. data is read from the file
					/// </summary>
					Play
				};

				/// <summary>
				/// Constructor
				/// </summary>
				/// <remarks>
				/// Creates a new WaveFile in <see cref="Mode.Closed"/> mode.
				WaveFile();

				/// <summary>
				/// Constructor.
				/// </summary>
				/// <param name="fileName">Path to the file.</param>
				/// <remarks>
				/// Creates a new WaveFile instance and opens the specified file in <see cref="Mode.Play"/>.
				/// </remarks>
				WaveFile(System::String^ fileName);

				/// <summary>
				/// Constructor.
				/// </summary>
				/// <param name="fileName">Path to the file.</param>
				/// <param name="format">Defines the wave format.</param>
				/// <remarks>
				/// Creates a new WaveFile and opens the specified file in <see cref="Mode.Record"/>. A file header is written based 
				/// on the <paramref name="format"/>
				/// </remarks>
				WaveFile(System::String^ fileName, WaveFormat^ format);

				/// <summary>
				/// Closes the WaveFile, if <see cref="Close"/> has not been called yet.
				/// </summary>
				~WaveFile();

				/// <summary>
				/// Calculates the playback duration of the audio data based on the total sample count and a <paramref name="sampleRate"/>
				/// </summary>
				/// <param name="sampleRate">Number of samples per second.</param>
				/// <returns>Duration</returns>
				System::TimeSpan GetLength(double sampleRate);

				/// <summary>
				/// Calculates the playback position of the audio data based on the to current playback position and a <paramref name="sampleRate"/>
				/// </summary>
				/// <param name="sampleRate">Number of samples per second.</param>
				/// <returns>Position</returns>
				System::TimeSpan GetPosition(double sampleRate);

				/// <summary>
				/// Sets the playback position of the audio data based on a <see cref="TimeSpan"/> and a <paramref name="sampleRate"/>
				/// </summary>
				/// <param name="sampleRate">Number of samples per second.</param>
				/// <param name="value">The play position to assign.</param>
				/// <exception cref="ArgumentOutOfRangeException">Thrown if the <paramref name="value"/> is greater than the playback duration.
				/// <seealso cref="GetLength"/>
				void SetPosition(double sampleRate, System::TimeSpan value);

				/// <summary>
				/// Creates a file with the provided <paramref name="fileName"/> and <paramref name="format"/> in <see cref="Mode.Record"/>
				/// </summary>
				/// <param name="fileName">Path of the file to create.</param>
				/// <param name="format">Wave data format</param>
				void Create(System::String^ fileName, WaveFormat^ format);

				/// <summary>
				/// Opens the file under the provided <paramref name="fileName"/> in <see cref="Mode.Play"/>
				/// </summary>
				/// <param name="fileName">Path of the file to read.</param>
				void Open(System::String^ fileName);

				/// <summary>
				/// Reads <paramref name="count"/> samples from the WaveFile into <paramref name="data"/> buffer at the given <paramref name="offset"/>. 
				/// </summary>
				/// <param name="data">Target sample buffer</param>
				/// <param name="offset">Offset in target buffer for the first sample copied from the file.</param>
				/// <param name="count">Number of samples to copy.</param>
				/// <returns>Number of samples written to <paramref name="data"/>.</returns>
				/// <remarks>The WaveFile must be opened in <see cref="Mode.Play"/> mode.</remarks>
				int ReadSamples(cli::array<System::Single>^ data, int offset, int count);

				/// <summary>
				/// Writes <paramref name="count"/> samples from <paramref name="data"/> buffer at <paramref name="offset"/> to the WaveFile.
				/// </summary>
				/// <param name="data">Sourc sample buffer</param>
				/// <param name="offset">Offset in source buffer of the first sample to copy to the file.</param>
				/// <param name="count">Number of samples to copy.</param>
				/// <returns>Number of samples written to the file.</returns>
				/// <remarks>The WaveFile must be opened in <see cref="Mode.Record"/> mode.</remarks>
				void WriteSamples(cli::array<System::Single>^ data, int offset, int count);

				/// <summary>
				/// Moves the file pointer to start position of sample data.
				/// </summary>
				void MoveToSamples();

				/// <summary>
				/// Closes the WaveFile
				/// </summary>
				/// <remarks>
				/// If the WaveFile was opened in <see cref="Mode.Record"/> the <see cref="WaveFormat.SampleCount"/> field will 
				/// be updated according to the actual amount of sample data written and the format header information in the
				/// file will be written.
				/// </remarks>
				void Close();

				/// <summary>
				/// Gets or sets the wave format header information.
				/// </summary>
				property WaveFormat^ Format
				{
					WaveFormat^ get();
					void set(WaveFormat^ value);
				}

				/// <summary>
				/// Gets a value indicating whether in <see cref="Mode.Play"/> the end of the WaveFile is reached.
				/// </summary>
				property System::Boolean IsEOF
				{
					System::Boolean get();
				}

				/// <summary>
				/// Gets the <see cref="Mode"/> in which the WaveFile was opened.
				/// </summary>
				property Mode CurrentMode
				{
					Mode get();
				}

				/// <summary>
				/// Calculates the number of samples required to play audio at the given <paramref name="sampleRate"/> for the specified time.
				/// </summary>
				/// <param name="sampleRate">Sample rate in samples per second</param>
				/// <param name="value">Duration</param>
				/// <returns>Number of samples</returns>
				static long long TimeSpanToSamples(double sampleRate, System::TimeSpan value);

				/// <summary>
				/// Calculates the duration of audio playback at the given <paramref name="sampleRate"/> with the specified number of <paramref name="samples"/>.
				/// </summary>
				/// <param name="sampleRate">Sample rate in samples per second</param>
				/// <param name="samples">Number of samples</param>
				/// <returns>Duration</returns>
				static System::TimeSpan SamplesToTimeSpan(double sampleRate, long long samples);

			private:
				int WriteWavHeader(WaveFormat^ format);
				int ReadWavHeader();
				int GetTotalSampleCount();
				int GetSampleCountAtCurrentPosition();

				Mode m_mode;
				System::Nullable<int> m_samplesRecorded;

				WaveFormat^ m_wavFormat;
				System::IO::FileStream^ m_wavStream;
				System::IO::BufferedStream^ m_bufferedStream;
				System::IO::BinaryReader^ m_binaryReader;
				System::IO::BinaryWriter^ m_binaryWriter;

				Audio::Foundation::SampleReader^ m_sampleReader;
				Audio::Foundation::SampleWriter^ m_sampleWriter;

				System::Threading::Mutex^ m_lock;

				long long m_dataOffset;

				float ReadPCM16();
				void WritePCM16(float value);
				float ReadPCM32();
				void WritePCM32(float value);
				float ReadFloat();
				void WriteFloat(float value);
			};
		}
	}
}