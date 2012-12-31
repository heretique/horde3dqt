// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/legal/epl-v10.html
//
// *************************************************************************************************

#ifndef _egRenderers_H_
#define _egRenderers_H_

#include "egPrerequisites.h"
#include "egRenderDeviceBase.h"
#include "egRendererBase.h"
#include <string>
#include <vector>
#include <map>

namespace Horde3D {


	typedef IGPUTimer* (*GPUTimerFactoryFunc)();
	typedef IRenderer* (*RendererFactoryFunc)();

	struct RendererRegistryEntry
	{
		std::string				typeString;
		GPUTimerFactoryFunc		gpuTimerFactoryFunc;
		RendererFactoryFunc		rdFactoryFunc;
	};

	class RenderManager
	{
	public:
		~RenderManager();
		void registerRenderer(int rendererType, const std::string &typeString, GPUTimerFactoryFunc gtf,
			RendererFactoryFunc rdf);

		IRenderer* createRenderer(int type);
		IRenderer* createRenderer(const std::string &typeString);

		IGPUTimer* createGPUTimer(int type);
		IGPUTimer* createGPUTimer(const std::string &typeString);

	protected:
		std::map<int, RendererRegistryEntry> _registry;
	};

}

#endif // _egRenderers_H_
