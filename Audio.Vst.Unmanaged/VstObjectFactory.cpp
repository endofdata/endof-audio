#include "pch.h"
#include "VstObjectFactory.h"
#include "VstHostApplication.h"
#include "VstHost.h"

using namespace Audio::Vst::Unmanaged;

IVstHostPtr VstObjectFactory::CreateVstHost(const wchar_t* pwcszName, int sampleCount, double sampleRate)
{
	IHostApplicationPtr application = new VstHostApplication(pwcszName);
	return new VstHost(application, sampleCount, sampleRate);
}

VstObjectFactory::VstObjectFactory()
{
}