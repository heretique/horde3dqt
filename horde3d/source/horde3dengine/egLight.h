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

#ifndef _egLight_H_
#define _egLight_H_

#include "egPrerequisites.h"
#include "egMaterial.h"
#include "egScene.h"


namespace Horde3D {

// =================================================================================================
// Light Node
// =================================================================================================

struct LightNodeParams
{
	enum List
	{
		MatResI = 500,
		RadiusF,
		FovF,
		ColorF3,
		ColorMultiplierF,
		ShadowMapCountI,
		ShadowSplitLambdaF,
		ShadowMapBiasF,
		LightingContextStr,
		ShadowContextStr
	};
};

// =================================================================================================

struct LightNodeTpl : public SceneNodeTpl
{
	PMaterialResource  matRes;
	std::string        lightingContext, shadowContext;
	float              radius, fov;
	float              col_R, col_G, col_B, colMult;
	uint32             shadowMapCount;
	float              shadowSplitLambda;
	float              shadowMapBias;

	LightNodeTpl( const std::string &name, MaterialResource *materialRes,
	              const std::string &lightingContext, const std::string &shadowContext ) :
		SceneNodeTpl( SceneNodeTypes::Light, name ), matRes( materialRes ),
		lightingContext( lightingContext ), shadowContext( shadowContext ),
		radius( 100 ), fov( 90 ), col_R( 1 ), col_G( 1 ), col_B( 1 ), colMult( 1 ),
		shadowMapCount( 0 ), shadowSplitLambda( 0.5f ), shadowMapBias( 0.005f )
	{
	}
};

// =================================================================================================

class LightNode : public SceneNode
{
public:
	static SceneNodeTpl *parsingFunc( std::map<std::string, std::string > &attribs );
	static SceneNode *factoryFunc( const SceneNodeTpl &nodeTpl );
	
	int getParamI( int param );
	void setParamI( int param, int value );
	float getParamF( int param, int compIdx );
	void setParamF( int param, int compIdx, float value );
	const char *getParamStr( int param );
	void setParamStr( int param, const char *value );

	void calcScreenSpaceAABB( const Matrix4f &mat, float &x, float &y, float &w, float &h );

	inline const Frustum &getFrustum() { return _frustum; }
	inline const Matrix4f &getViewMat() { return _viewMat; }

	inline const Vec3f& absolutePos() { return _absPos; }
	inline const Vec3f& spotDirection() { return _spotDir; }
	inline const Vec3f& diffuseColor() { return _diffuseCol; }
	inline float radius() { return _radius; }
	inline float fov() { return _fov; }
	inline float diffuseColorMult() { return _diffuseColMult; }
	inline float shadowMapBias() { return _shadowMapBias; }
	inline float shadowSplitLambda() { return _shadowSplitLambda; }
	inline PMaterialResource materialRes() { return _materialRes; }
	inline uint32 shadowMapCount() { return _shadowMapCount; }
	inline const std::string& shadowContext() { return _shadowContext; }
	inline const std::string& lightingContext() { return _lightingContext; }
	inline std::vector< uint32 >& occQueries() { return _occQueries; }
	inline std::vector< uint32 >& lastVisited() { return _lastVisited; }

private:
	LightNode( const LightNodeTpl &lightTpl );
	~LightNode();

	void onPostUpdate();

private:
	Frustum                _frustum;
	Matrix4f               _viewMat;
	Vec3f                  _absPos, _spotDir;

	PMaterialResource      _materialRes;
	std::string            _lightingContext, _shadowContext;
	float                  _radius, _fov;
	Vec3f                  _diffuseCol;
	float                  _diffuseColMult;
	uint32                 _shadowMapCount;
	float                  _shadowSplitLambda, _shadowMapBias;

	std::vector< uint32 >  _occQueries;
	std::vector< uint32 >  _lastVisited;

	friend class SceneManager;
};

}
#endif // _egLight_H_
