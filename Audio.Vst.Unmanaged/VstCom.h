#pragma once

#include <ivstcomponent.h>
#include <ipluginbase.h>
#include <ivstaudioprocessor.h>
#include <ivsthostapplication.h>
#include <smartpointer.h>

typedef Steinberg::IPtr<Steinberg::IPluginBase> IPluginBasePtr;
typedef Steinberg::IPtr<Steinberg::IPluginFactory> IPluginFactoryPtr;
typedef Steinberg::IPtr<Steinberg::IPluginFactory2> IPluginFactory2Ptr;
typedef Steinberg::IPtr<Steinberg::IPluginFactory3> IPluginFactory3Ptr;
typedef Steinberg::IPtr<Steinberg::Vst::IComponent> IComponentPtr;
typedef Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> IAudioProcessorPtr;
typedef Steinberg::IPtr<Steinberg::Vst::IHostApplication> IHostApplicationPtr;
