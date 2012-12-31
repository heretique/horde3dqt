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

#include "egRenderManager.h"
#include <utility>



namespace Horde3D {

	RenderManager::~RenderManager()
	{

	}


	void RenderManager::registerRenderer(int rendererType, const std::string &typeString, GPUTimerFactoryFunc gtf,
		RendererFactoryFunc rdf)
	{
		RendererRegistryEntry entry;
		entry.typeString = typeString;
		entry.gpuTimerFactoryFunc = gtf;
		entry.rdFactoryFunc = rdf;
		_registry[rendererType] = entry;
	}

	IRenderer* RenderManager::createRenderer(int type)
	{
		std::map<int, RendererRegistryEntry>::iterator itr = _registry.find( type );
		if( itr != _registry.end() ) return itr->second.rdFactoryFunc();
		else return 0x0;
	}

	IRenderer* RenderManager::createRenderer( const std::string &typeString )
	{
		std::map<int, RendererRegistryEntry>::iterator itr = _registry.begin();
		while (itr != _registry.end())
		{
			if (itr->second.typeString == typeString) return itr->second.rdFactoryFunc();
			++itr;
		}

		return 0x0;
	}

	IGPUTimer* RenderManager::createGPUTimer(int type)
	{
		std::map<int, RendererRegistryEntry>::iterator itr = _registry.find( type );
		if( itr != _registry.end() ) return itr->second.gpuTimerFactoryFunc();
		else return 0x0;
	}

	IGPUTimer* RenderManager::createGPUTimer( const std::string &typeString )
	{
		std::map<int, RendererRegistryEntry>::iterator itr = _registry.begin();
		while (itr != _registry.end())
		{
			if (itr->second.typeString == typeString) return itr->second.gpuTimerFactoryFunc();
			++itr;
		}

		return 0x0;
	}

}  // namespace
