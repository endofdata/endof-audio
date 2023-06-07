#include "pch.h"

#include "Looper.h"
#include "LooperConfig.h"
#include "AsioObjectFactory.h"

using namespace Audio::Asio::Unmanaged;


ILooperConfigPtr AsioObjectFactory::CreateLooperConfiguration()
{
	return new LooperConfig();
}

ILooperPtr AsioObjectFactory::CreateLooper(const ILooperConfigPtr& config)
{
	return Looper::Create(config);
}
