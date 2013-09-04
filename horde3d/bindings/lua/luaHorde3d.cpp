#include "luaHorde3d.h"
#include "Horde3D.h"
#include "Horde3DUtils.h"

#include "lua.hpp"

#define ADD_CONSTANT(val, name) lua_pushnumber(L,val); \
    lua_setfield(L, -2, name);

void addH3DRenderersConsts(lua_State* L)
{
    lua_newtable(L);
    ADD_CONSTANT(H3DRenderers::OpenGL_2_0, "OpenGL_2_0");
    ADD_CONSTANT(H3DRenderers::OpenGL_ES_2_0, "OpenGL_ES_2_0");
    ADD_CONSTANT(H3DRenderers::OpenGL_ES_3_0, "OpenGL_ES_3_0");

    lua_setglobal(L, "H3DRenderers");
}

void addH3DOptionsConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DOptions::MaxLogLevel;
    ADD_CONSTANT(counter++, "MaxLogLevel");
    ADD_CONSTANT(counter++, "MaxNumMessages");
    ADD_CONSTANT(counter++, "TrilinearFiltering");
    ADD_CONSTANT(counter++, "MaxAnisotropy");
    ADD_CONSTANT(counter++, "TexCompression");
    ADD_CONSTANT(counter++, "SRGBLinearization");
    ADD_CONSTANT(counter++, "LoadTextures");
    ADD_CONSTANT(counter++, "FastAnimation");
    ADD_CONSTANT(counter++, "ShadowMapSize");
    ADD_CONSTANT(counter++, "SampleCount");
    ADD_CONSTANT(counter++, "WireframeMode");
    ADD_CONSTANT(counter++, "DebugViewMode");
    ADD_CONSTANT(counter++, "DumpFailedShaders");
    ADD_CONSTANT(counter++, "GatherTimeStats")
    lua_setglobal(L, "H3DOptions");
}

void addH3DStatsConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DStats::TriCount;
    ADD_CONSTANT(counter++, "TriCount");
    ADD_CONSTANT(counter++, "BatchCount");
    ADD_CONSTANT(counter++, "LightPassCount");
    ADD_CONSTANT(counter++, "FrameTime");
    ADD_CONSTANT(counter++, "AnimationTime");
    ADD_CONSTANT(counter++, "GeoUpdateTime");
    ADD_CONSTANT(counter++, "ParticleSimTime");
    ADD_CONSTANT(counter++, "FwdLightsGPUTime");
    ADD_CONSTANT(counter++, "DefLightsGPUTime");
    ADD_CONSTANT(counter++, "ShadowsGPUTime");
    ADD_CONSTANT(counter++, "ParticleGPUTime");
    ADD_CONSTANT(counter++, "TextureVMem");
    ADD_CONSTANT(counter++, "GeometryVMem");
    lua_setglobal(L, "H3DStats");
}

void addH3DResTypesConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DResTypes::Undefined;
    ADD_CONSTANT(counter++, "Undefined");
    ADD_CONSTANT(counter++, "SceneGraph");
    ADD_CONSTANT(counter++, "Geometry");
    ADD_CONSTANT(counter++, "Animation");
    ADD_CONSTANT(counter++, "Material");
    ADD_CONSTANT(counter++, "Code");
    ADD_CONSTANT(counter++, "Shader");
    ADD_CONSTANT(counter++, "Texture");
    ADD_CONSTANT(counter++, "ParticleEffect");
    ADD_CONSTANT(counter++, "Pipeline");
    lua_setglobal(L, "H3DResTypes");
}

void addH3DResFlagsConsts(lua_State* L)
{
    lua_newtable(L);
    ADD_CONSTANT(H3DResFlags::NoQuery, "NoQuery");
    ADD_CONSTANT(H3DResFlags::NoTexCompression, "NoTexCompression");
    ADD_CONSTANT(H3DResFlags::NoTexMipmaps, "NoTexMipmaps");
    ADD_CONSTANT(H3DResFlags::TexCubemap, "TexCubemap");
    ADD_CONSTANT(H3DResFlags::TexDynamic,  "TexDynamic");
    ADD_CONSTANT(H3DResFlags::TexRenderable, "TexRenderable");
    ADD_CONSTANT(H3DResFlags::TexSRGB,  "TexSRGB");

    lua_setglobal(L, "H3DResFlags");
}

void addH3DFormatsConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DFormats::Unknown;
    ADD_CONSTANT(counter++, "Unknown");
    ADD_CONSTANT(counter++, "TEX_BGRA8");
    ADD_CONSTANT(counter++, "TEX_DXT1");
    ADD_CONSTANT(counter++, "TEX_DXT3");
    ADD_CONSTANT(counter++, "TEX_DXT5");
    ADD_CONSTANT(counter++, "TEX_RGBA16F");
    ADD_CONSTANT(counter++, "TEX_RGBA32F");
    lua_setglobal(L, "H3DFormats");
}

void addH3DGeoResConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DGeoRes::GeometryElem;
    ADD_CONSTANT(counter++, "GeometryElem");
    ADD_CONSTANT(counter++, "GeoIndexCountI");
    ADD_CONSTANT(counter++, "GeoVertexCountI");
    ADD_CONSTANT(counter++, "GeoIndices16I");
    ADD_CONSTANT(counter++, "GeoIndexStream");
    ADD_CONSTANT(counter++, "GeoVertPosStream");
    ADD_CONSTANT(counter++, "GeoVertTanStream");
    ADD_CONSTANT(counter++, "GeoVertStaticStream")
    lua_setglobal(L, "H3DGeoRes");
}

void addH3DAnimResConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DAnimRes::EntityElem;
    ADD_CONSTANT(counter++, "EntityElem");
    ADD_CONSTANT(counter++, "EntFrameCountI");
    lua_setglobal(L, "H3DAnimRes");
}

void addH3DMatResConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DMatRes::MaterialElem;
    ADD_CONSTANT(counter++, "MaterialElem");
    ADD_CONSTANT(counter++, "SamplerElem");
    ADD_CONSTANT(counter++, "UniformElem");
    ADD_CONSTANT(counter++, "MatClassStr");
    ADD_CONSTANT(counter++, "MatLinkI");
    ADD_CONSTANT(counter++, "MatShaderI");
    ADD_CONSTANT(counter++, "SampNameStr");
    ADD_CONSTANT(counter++, "SampTexResI");
    ADD_CONSTANT(counter++, "UnifNameStr");
    ADD_CONSTANT(counter++, "UnifValueF4");
    lua_setglobal(L, "H3DMatRes");

}

void addH3DShaderResConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DShaderRes::ContextElem;
    ADD_CONSTANT(counter++, "ContextElem");
    ADD_CONSTANT(counter++, "UniformElem");
    ADD_CONSTANT(counter++, "ContNameStr");
    ADD_CONSTANT(counter++, "SampNameStr");
    ADD_CONSTANT(counter++, "SampDefTexResI");
    ADD_CONSTANT(counter++, "UnifNameStr");
    ADD_CONSTANT(counter++, "UnifSizeI");
    ADD_CONSTANT(counter++, "UnifDefValueF4");
    lua_setglobal(L, "H3DShaderRes");
}

void addH3DTexResConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DTexRes::TextureElem;
    ADD_CONSTANT(counter++, "TextureElem");
    ADD_CONSTANT(counter++, "ImageElem");
    ADD_CONSTANT(counter++, "TexFormatI");
    ADD_CONSTANT(counter++, "TexSliceCountI");
    ADD_CONSTANT(counter++, "ImgWidthI");
    ADD_CONSTANT(counter++, "ImgHeightI");
    ADD_CONSTANT(counter++, "ImgPixelStream");
    lua_setglobal(L, "H3DTexRes");
}

void addH3DPartEffResConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DPartEffRes::ParticleElem;
    ADD_CONSTANT(counter++, "ParticleElem");
    ADD_CONSTANT(counter++, "ChanMoveVelElem");
    ADD_CONSTANT(counter++, "ChanRotVelElem");
    ADD_CONSTANT(counter++, "ChanSizeElem");
    ADD_CONSTANT(counter++, "ChanColRElem");
    ADD_CONSTANT(counter++, "ChanColGElem");
    ADD_CONSTANT(counter++, "ChanColBElem");
    ADD_CONSTANT(counter++, "ChanColAElem");
    ADD_CONSTANT(counter++, "PartLifeMinF");
    ADD_CONSTANT(counter++, "PartLifeMaxF");
    ADD_CONSTANT(counter++, "ChanStartMinF");
    ADD_CONSTANT(counter++, "ChanStartMaxF");
    ADD_CONSTANT(counter++, "ChanEndRateF");
    ADD_CONSTANT(counter++, "ChanDragElem");
    lua_setglobal(L, "H3DPartEffRes");
}

void addH3DPipeResConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DPipeRes::StageElem;
    ADD_CONSTANT(counter++, "StageElem");
    ADD_CONSTANT(counter++, "StageNameStr");
    ADD_CONSTANT(counter++, "StageActivationI");
    lua_setglobal(L, "H3DPipeRes");
}

void addH3DNodeTypesConsts(lua_State* L)
{
    lua_newtable(L);

    int counter=H3DNodeTypes::Undefined;
    ADD_CONSTANT(counter++, "Undefined");
    ADD_CONSTANT(counter++, "Group");
    ADD_CONSTANT(counter++, "Model");
    ADD_CONSTANT(counter++, "Mesh");
    ADD_CONSTANT(counter++, "Joint");
    ADD_CONSTANT(counter++, "Light");
    ADD_CONSTANT(counter++, "Camera");
    ADD_CONSTANT(counter++, "Emitter");

    lua_setglobal(L, "H3DNodeTypes");
}

void addH3DNodeFlagsConsts(lua_State* L)
{
    lua_newtable(L);
    ADD_CONSTANT(H3DNodeFlags::NoDraw, "NoDraw");
    ADD_CONSTANT(H3DNodeFlags::NoCastShadow, "NoCastShadow");
    ADD_CONSTANT(H3DNodeFlags::NoRayQuery, "NoRayQuery");
    ADD_CONSTANT(H3DNodeFlags::Inactive, "Inactive");
    lua_setglobal(L, "H3DNodeFlags");
}

void addH3DNodeParamsConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DNodeParams::NameStr;
    ADD_CONSTANT(counter++, "NameStr");
    ADD_CONSTANT(counter++, "AttachmentStr");
    lua_setglobal(L, "H3DNodeParams");
}


void addH3DModelConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DModel::GeoResI;
    ADD_CONSTANT(counter++, "GeoResI");
    ADD_CONSTANT(counter++, "SWSkinningI");
    ADD_CONSTANT(counter++, "LodDist1F");
    ADD_CONSTANT(counter++, "LodDist2F");
    ADD_CONSTANT(counter++, "LodDist3F");
    ADD_CONSTANT(counter++, "LodDist4F");
    lua_setglobal(L, "H3DModel");
}

void addH3DMeshConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DMesh::MatResI;
    ADD_CONSTANT(counter++, "MatResI");
    ADD_CONSTANT(counter++, "BatchStartI");
    ADD_CONSTANT(counter++, "BatchCountI");
    ADD_CONSTANT(counter++, "VertRStartI");
    ADD_CONSTANT(counter++, "VertREndI");
    ADD_CONSTANT(counter++, "LodLevelI");
    lua_setglobal(L, "H3DMesh");
}

void addH3DJointConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DJoint::JointIndexI;
    ADD_CONSTANT(counter++, "JointIndexI");
    lua_setglobal(L, "H3DJoint");
}

void addH3DLightConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DLight::MatResI;
    ADD_CONSTANT(counter++, "MatResI");
    ADD_CONSTANT(counter++, "RadiusF");
    ADD_CONSTANT(counter++, "FovF");
    ADD_CONSTANT(counter++, "ColorF3");
    ADD_CONSTANT(counter++, "ColorMultiplierF");
    ADD_CONSTANT(counter++, "ShadowMapCountI");
    ADD_CONSTANT(counter++, "ShadowSplitLambdaF");
    ADD_CONSTANT(counter++, "ShadowMapBiasF");
    ADD_CONSTANT(counter++, "LightingContextStr");
    ADD_CONSTANT(counter++, "ShadowContextStr");
    lua_setglobal(L, "H3DLight");
}

void addH3DCameraConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DCamera::PipeResI;
    ADD_CONSTANT(counter++, "PipeResI");
    ADD_CONSTANT(counter++, "OutTexResI");
    ADD_CONSTANT(counter++, "OutBufIndexI");
    ADD_CONSTANT(counter++, "LeftPlaneF");
    ADD_CONSTANT(counter++, "RightPlaneF");
    ADD_CONSTANT(counter++, "BottomPlaneF");
    ADD_CONSTANT(counter++, "TopPlaneF");
    ADD_CONSTANT(counter++, "NearPlaneF");
    ADD_CONSTANT(counter++, "FarPlaneF");
    ADD_CONSTANT(counter++, "ViewportXI");
    ADD_CONSTANT(counter++, "ViewportYI");
    ADD_CONSTANT(counter++, "ViewportWidthI");
    ADD_CONSTANT(counter++, "ViewportHeightI");
    ADD_CONSTANT(counter++, "OrthoI");
    ADD_CONSTANT(counter++, "OccCullingI");
    lua_setglobal(L, "H3DCamera");
}

void addH3DEmitterConsts(lua_State* L)
{
    lua_newtable(L);
    int counter=H3DEmitter::MatResI;
    ADD_CONSTANT(counter++, "MatResI");
    ADD_CONSTANT(counter++, "PartEffResI");
    ADD_CONSTANT(counter++, "MaxCountI");
    ADD_CONSTANT(counter++, "RespawnCountI");
    ADD_CONSTANT(counter++, "DelayF");
    ADD_CONSTANT(counter++, "EmissionRateF");
    ADD_CONSTANT(counter++, "SpreadAngleF");
    ADD_CONSTANT(counter++, "ForceF3");
    lua_setglobal(L, "H3DEmitter");
}

void addH3DModelUpdateFlagsConsts(lua_State* L)
{
    lua_newtable(L);
    ADD_CONSTANT(H3DModelUpdateFlags::Animation, "Animation");
    ADD_CONSTANT(H3DModelUpdateFlags::Geometry, "Geometry");
    lua_setglobal(L, "H3DModelUpdateFlags");
}

static int lua_h3dGetVersionString(lua_State* L)
{
    lua_pushstring(L, h3dGetVersionString());
    return 1;
}

static int lua_h3dCheckExtension(lua_State* L)
{
    bool result=false;
    if(lua_gettop(L)!=1 || lua_isstring(L, 1)==false)
    {
        return 0;
    }

    const char* ext=lua_tostring(L, 1);
    lua_pop(L, 1);

    result=h3dCheckExtension(ext);
    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dGetError(lua_State* L)
{
    bool result=false;
    result=h3dGetError();
    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dInit(lua_State* L)
{
    float result;
    int param;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    param=lua_tonumber(L, 1);
    lua_pop(L, 1);
    result=h3dInit((H3DRenderers::List)param);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dRelease(lua_State* L)
{
    h3dRelease();
    return 0;
}

static int lua_h3dRender(lua_State* L)
{
    int cameraHandle;
    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    cameraHandle=lua_tonumber(L,1);
    lua_pop(L, 1);

    h3dRender(cameraHandle);
    return 0;
}

static int lua_h3dFinalizeFrame(lua_State* L)
{
    h3dFinalizeFrame();
    return 0;
}

static int lua_h3dClear(lua_State* L)
{
    h3dClear();
    return 0;
}

static int lua_h3dGetMessage(lua_State* L)
{
    int level;
    float time;
    const char* message;

    message=h3dGetMessage(&level, &time);
    lua_pushstring(L,message);
    lua_pushnumber(L, level);
    lua_pushnumber(L, time);

    return 3;
}

static int lua_h3dGetOption(lua_State* L)
{
    float result;
    int param;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    param=lua_tonumber(L, 1);
    lua_pop(L, 1);
    result=h3dGetOption((H3DOptions::List)param);

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetOption(lua_State* L)
{
    bool result;
    int param;
    float value;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    param=lua_tonumber(L, 1);
    value=lua_tonumber(L, 2);
    lua_pop(L, 2);

    result=h3dSetOption((H3DOptions::List)param, value);

    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dGetStat(lua_State* L)
{
    float result;
    bool reset;
    int param;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    param=lua_tonumber(L, 1);
    reset=lua_toboolean(L, 2);
    lua_pop(L,2);

    result=h3dGetStat((H3DStats::List)param, reset);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dShowOverlays(lua_State* L)
{
    // BIG WARNING THIS WILL CERTAINLY CRASH  HOW TO GET POINTER FROM LUA?
    const float *verts;
    int vertCount, flags;
    float colR,colG,colB, colA;
    H3DRes materialRes;

    if(lua_gettop(L)!=8)
    {
        return 0;
    }

    verts=(const float*)lua_touserdata(L,1);
    vertCount=lua_tonumber(L,2);
    colR=lua_tonumber(L,3);
    colG=lua_tonumber(L,4);
    colB=lua_tonumber(L,5);
    colA=lua_tonumber(L,6);
    materialRes=lua_tonumber(L,7);
    flags=lua_tonumber(L,8);
    lua_pop(L, 8);

    h3dShowOverlays(verts, vertCount, colR, colG, colB, colA, materialRes, flags);

    return 0;
}

static int lua_h3dClearOverlays(lua_State* L)
{
    h3dClearOverlays();
    return 0;
}

static int lua_h3dGetResType(lua_State* L)
{
    int resource;
    int result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    resource=lua_tonumber(L, 1);
    lua_pop(L,1);

    result=h3dGetResType(resource);
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dGetResName(lua_State* L)
{
    int resource;
    const char* result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    resource=lua_tonumber(L, 1);
    lua_pop(L,1);

    result=h3dGetResName(resource);
    lua_pushstring(L, result);

    return 1;
}

static int lua_h3dGetNextResource(lua_State* L)
{
    int type;
    H3DRes start, result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    type=lua_tonumber(L, 1);
    start = (H3DRes)lua_tonumber(L, 2);
    lua_pop(L,2);

    result=h3dGetNextResource(type, start);
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dFindResource(lua_State* L)
{
    int type;
    const char* name;
    H3DRes result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    type=lua_tonumber(L, 1);
    name=lua_tostring(L, 2);
    lua_pop(L, 2);

    result=(H3DRes)h3dFindResource(type, name);
    if(result==0)
    {
        return 0;
    }

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dAddResource(lua_State* L)
{
    int type;
    const char* name;
    int flags;
    H3DRes result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    type=lua_tonumber(L, 1);
    name=lua_tostring(L, 2);
    flags=lua_tonumber(L, 3);
    lua_pop(L, 3);

    result=(H3DRes)h3dAddResource(type, name, flags);
    lua_pushnumber(L,result);
    return 1;
}

static int lua_h3dCloneResource(lua_State* L)
{
    const char* name;
    H3DRes result, originalHandle;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    originalHandle=(H3DRes)lua_tonumber(L, 1);
    name=lua_tostring(L, 2);
    lua_pop(L, 2);

    result=(H3DRes)h3dCloneResource(originalHandle, name);

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dRemoveResource(lua_State* L)
{
    H3DRes handle;
    int result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=(H3DRes)lua_tonumber(L, 1);
    lua_pop(L, 1);

    result=h3dRemoveResource(handle);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dIsResLoaded(lua_State* L)
{
    H3DRes handle;
    bool result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=(H3DRes)lua_tonumber(L, 1);
    lua_pop(L, 1);

    result=h3dIsResLoaded(handle);

    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dLoadResource(lua_State* L)
{
    H3DRes handle;
    const char* data;
    int size;
    bool result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    handle=(H3DRes)lua_tonumber(L, 1);
    data=(const char*)lua_touserdata(L, 2);
    size=lua_tonumber(L, 3);
    lua_pop(L, 3);

    result=h3dLoadResource(handle, data, size);

    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dUnloadResource(lua_State* L)
{
    H3DRes handle;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=(H3DRes)lua_tonumber(L, 1);
    lua_pop(L, 1);

    h3dUnloadResource(handle);

    return 0;
}

static int lua_h3dGetResElemCount(lua_State* L)
{
    H3DRes handle;
    int elem;
    int result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    handle=(H3DRes)lua_tonumber(L, 1);
    elem = lua_tonumber(L, 2);
    lua_pop(L, 2);

    result = h3dGetResElemCount(handle, elem);
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dFindResElem(lua_State* L)
{
    H3DRes res;
    int elem, param, result;
    const char *name;

    if(lua_gettop(L)!=4)
    {
        return 0;
    }

    res=(H3DRes)lua_tonumber(L, 1);
    elem = lua_tonumber(L, 2);
    param = lua_tonumber(L, 3);
    name = lua_tostring(L, 4);
    lua_pop(L, 4);

    result = h3dFindResElem( res, elem, param, name );
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dGetResParamI(lua_State* L)
{
    H3DRes res;
    int elem, elemIdx, param, result;

    if(lua_gettop(L)!=4)
    {
        return 0;
    }

    res=lua_tonumber(L, 1);
    elem=lua_tonumber(L, 2);
    elemIdx=lua_tonumber(L, 3);
    param=lua_tonumber(L, 4);
    lua_pop(L, 4);

    result=h3dGetResParamI(res, elem, elemIdx, param);

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetResParamI(lua_State* L)
{
    H3DRes res;
    int elem, elemIdx, param, value;

    if(lua_gettop(L)!=5)
    {
        return 0;
    }

    res=lua_tonumber(L, 1);
    elem=lua_tonumber(L, 2);
    elemIdx=lua_tonumber(L, 3);
    param=lua_tonumber(L, 4);
    value=lua_tonumber(L, 5);
    lua_pop(L, 5);

    h3dSetResParamI(res, elem, elemIdx, param, value);
    return 0;
}

static int lua_h3dGetResParamF(lua_State* L)
{
    H3DRes res;
    int elem, elemIdx, param, compIdx;
    float result;

    if(lua_gettop(L)!=5)
    {
        return 0;
    }

    res=lua_tonumber(L, 1);
    elem=lua_tonumber(L, 2);
    elemIdx=lua_tonumber(L, 3);
    param=lua_tonumber(L, 4);
    compIdx=lua_tonumber(L, 5);
    lua_pop(L, 5);

    result=h3dGetResParamF(res, elem, elemIdx, param, compIdx);

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetResParamF(lua_State* L)
{
    H3DRes res;
    int elem, elemIdx, param, compIdx, value;

    if(lua_gettop(L)!=6)
    {
        return 0;
    }

    res=lua_tonumber(L, 1);
    elem=lua_tonumber(L, 2);
    elemIdx=lua_tonumber(L, 3);
    param=lua_tonumber(L, 4);
    compIdx=lua_tonumber(L, 5);
    value = lua_tonumber(L, 6);
    lua_pop(L, 6);

    h3dSetResParamF(res, elem, elemIdx, param, compIdx, value);
    return 0;
}

static int lua_h3dGetResParamStr(lua_State* L)
{
    H3DRes res;
    int elem, elemIdx, param;
    const char*  result;

    if(lua_gettop(L)!=4)
    {
        return 0;
    }

    res=lua_tonumber(L, 1);
    elem=lua_tonumber(L, 2);
    elemIdx=lua_tonumber(L, 3);
    param=lua_tonumber(L, 4);
    lua_pop(L, 4);

    result=h3dGetResParamStr(res, elem, elemIdx, param);

    lua_pushstring(L,result);
    return 1;
}

static int lua_h3dSetResParamStr(lua_State* L)
{
    H3DRes res;
    int elem, elemIdx, param;
    const char *value;

    if(lua_gettop(L)!=5)
    {
        return 0;
    }

    res=lua_tonumber(L, 1);
    elem=lua_tonumber(L, 2);
    elemIdx=lua_tonumber(L, 3);
    param=lua_tonumber(L, 4);
    value=lua_tostring(L, 5);
    lua_pop(L, 5);

    h3dSetResParamStr(res, elem, elemIdx, param, value);

    return 0;
}

static int lua_h3dMapResStream(lua_State* L)
{
    H3DRes res;
    int elem, elemIdx, stream;
    bool read, write;
    void *userData;

    if(lua_gettop(L)!=6)
    {
        return 0;
    }
    res=lua_tonumber(L, 1);
    elem=lua_tonumber(L, 2);
    elemIdx=lua_tonumber(L, 3);
    stream=lua_tonumber(L, 4);
    read = lua_toboolean(L, 5);
    write = lua_toboolean(L, 6);
    lua_pop(L, 6);

    userData = h3dMapResStream(res, elem, elemIdx, stream, read, write);
    lua_pushlightuserdata(L, userData);

    return 1;
}

static int lua_h3dUnmapResStream(lua_State *L)
{
    H3DRes res;
    if(lua_gettop(L)!=1)
    {
        return 0;
    }
    res=lua_tonumber(L, 1);
    lua_pop(L, 1);
    h3dUnmapResStream(res);
    return 0;
}


static int lua_h3dQueryUnloadedResource(lua_State* L)
{
    int index;
    H3DRes res;
    if(lua_gettop(L)!=1)
    {
        return 0;
    }
    index=lua_tonumber(L, 1);
    lua_pop(L, 1);
    res = h3dQueryUnloadedResource(index);
    lua_pushnumber(L, res);
    return 1;
}

static int lua_h3dReleaseUnusedResources(lua_State* L)
{
    h3dReleaseUnusedResources();
    return 0;
}

static int lua_h3dCreateTexture(lua_State* L)
{
    const char* name;
    int flags, width, height, fmt;
    H3DRes result;

    if(lua_gettop(L)!=5)
    {
        return 0;
    }

    name=lua_tostring(L,1);
    width=lua_tonumber(L,2);
    height=lua_tonumber(L,3);
    fmt=lua_tonumber(L,4);
    flags=lua_toboolean(L,5);
    lua_pop(L,5);

    result=(H3DRes)h3dCreateTexture(name, width, height, fmt, flags);
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dSetShaderPreambles(lua_State* L)
{
    const char* vertPreamble;
    const char* fragPreamble;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    vertPreamble=lua_tostring(L, 1);
    fragPreamble=lua_tostring(L,2);
    lua_pop(L,2);

    h3dSetShaderPreambles(vertPreamble, fragPreamble);

    return 0;
}

static int lua_h3dSetMaterialUniform(lua_State* L)
{
    H3DRes handle;
    const char* name;
    float a,b,c,d;
    bool result;

    if(lua_gettop(L)!=6)
    {
        return 0;
    }

    handle=(H3DRes)lua_tonumber(L, 1);
    name=lua_tostring(L, 2);
    a=lua_tonumber(L,3);
    b=lua_tonumber(L,4);
    c=lua_tonumber(L,5);
    d=lua_tonumber(L,6);
    lua_pop(L,6);

    result=h3dSetMaterialUniform(handle, name, a,b,c,d);
    lua_pushboolean(L, result);

    return 1;
}

static int lua_h3dResizePipelineBuffers(lua_State* L)
{
    H3DRes res;
    int width, height;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    res=lua_tonumber(L,1);
    width=lua_tonumber(L,2);
    height=lua_tonumber(L,3);
    lua_pop(L,3);

    h3dResizePipelineBuffers(res, width, height);

    return 0;
}

static int lua_h3dGetRenderTargetData(lua_State* L)
{
    H3DRes pipelineRes;
    const char *targetName;
    int bufIndex;
    int width;
    int height;
    int compCount;
    void *dataBuffer;
    int bufferSize;
    bool result;


    if(lua_gettop(L)!=5)
    {
        return 0;
    }
    pipelineRes=lua_tonumber(L,1);
    targetName=lua_tostring(L,2);
    bufIndex=lua_tonumber(L,3);
    dataBuffer = lua_touserdata(L, 4);
    bufferSize = lua_tonumber(L, 5);
    lua_pop(L,5);
    result = h3dGetRenderTargetData( pipelineRes, targetName, bufIndex, &width, &height, &compCount, dataBuffer, bufferSize );
    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dGetNodeType(lua_State* L)
{
    H3DNode handle;
    int result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=(H3DNode)lua_tonumber(L, 1);
    lua_pop(L,1);

    result=h3dGetNodeType(handle);
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dGetNodeParent(lua_State* L)
{
    H3DNode handle;
    H3DNode result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=(H3DNode)lua_tonumber(L, 1);
    lua_pop(L,1);

    result=h3dGetNodeParent(handle);
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dSetNodeParent(lua_State* L)
{
    int handle;
    int parent;
    bool result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    parent=lua_tonumber(L, 2);
    lua_pop(L,2);

    result=h3dSetNodeParent(handle, parent);
    lua_pushboolean(L, result);

    return 1;
}

static int lua_h3dGetNodeChild(lua_State* L)
{
    int handle;
    int index;
    int result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    index=lua_tonumber(L, 2);
    lua_pop(L,2);

    result=h3dGetNodeChild(handle, index);
    lua_pushnumber(L, result);

    return 1;
}

static int lua_h3dAddNodes(lua_State* L)
{
    int parent;
    int sceneGraph;
    int result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    parent=lua_tonumber(L, 1);
    sceneGraph=lua_tonumber(L, 2);
    lua_pop(L,2);

    result=h3dAddNodes(parent, sceneGraph);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dRemoveNode(lua_State* L)
{
    int handle;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    lua_pop(L,1);

    h3dRemoveNode(handle);
    return 0;
}

static int lua_h3dCheckNodeTransFlag(lua_State* L)
{
    int handle;
    bool reset;
    bool result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    reset=lua_toboolean(L, 2);
    lua_pop(L,2);

    result=h3dCheckNodeTransFlag(handle, reset);
    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dGetNodeTransform(lua_State* L)
{
    int handle;
    float tx, ty, tz;
    float rx, ry, rz;
    float sx, sy, sz;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    lua_pop(L,1);

    h3dGetNodeTransform(handle, &tx, &ty, &tz,
                            &rx, &ry, &rz,
                            &sx, &sy, &sz);


    lua_pushnumber(L, tx);
    lua_pushnumber(L, ty);
    lua_pushnumber(L, tz);
    lua_pushnumber(L, rx);
    lua_pushnumber(L, ry);
    lua_pushnumber(L, rz);
    lua_pushnumber(L, sx);
    lua_pushnumber(L, sy);
    lua_pushnumber(L, sz);
    return 9;
}

static int lua_h3dSetNodeTransform(lua_State* L)
{
    int handle;
    float tx, ty, tz;
    float rx, ry, rz;
    float sx, sy, sz;
    if(lua_gettop(L)!=10)
    {
        return 0;
    }

    handle=lua_tointeger(L, 1);
    tx=lua_tonumber(L, 2);
    ty=lua_tonumber(L, 3);
    tz=lua_tonumber(L, 4);
    rx=lua_tonumber(L, 5);
    ry=lua_tonumber(L, 6);
    rz=lua_tonumber(L, 7);
    sx=lua_tonumber(L, 8);
    sy=lua_tonumber(L, 9);
    sz=lua_tonumber(L, 10);
    lua_pop(L,10);

    h3dSetNodeTransform(handle, tx, ty, tz,
                            rx, ry, rz,
                            sx, sy, sz);
    return 0;
}

static int lua_h3dGetNodeTransMats(lua_State* L)
{
    int node = luaL_checkint( L, 1 );
    const float *relMatrix = 0, *absMatrix;
    h3dGetNodeTransMats( node, &relMatrix, &absMatrix );

    lua_pushnumber( L, relMatrix[0] ); lua_pushnumber( L, relMatrix[1] ); lua_pushnumber( L, relMatrix[2] ); lua_pushnumber( L, relMatrix[3] );
    lua_pushnumber( L, relMatrix[4] ); lua_pushnumber( L, relMatrix[5] ); lua_pushnumber( L, relMatrix[6] ); lua_pushnumber( L, relMatrix[7] );
    lua_pushnumber( L, relMatrix[8] ); lua_pushnumber( L, relMatrix[9] ); lua_pushnumber( L, relMatrix[10] ); lua_pushnumber( L, relMatrix[11] );
    lua_pushnumber( L, relMatrix[12] ); lua_pushnumber( L, relMatrix[13] ); lua_pushnumber( L, relMatrix[14] ); lua_pushnumber( L, relMatrix[15] );

    lua_pushnumber( L, absMatrix[0] ); lua_pushnumber( L, absMatrix[1] ); lua_pushnumber( L, absMatrix[2] ); lua_pushnumber( L, absMatrix[3] );
    lua_pushnumber( L, absMatrix[4] ); lua_pushnumber( L, absMatrix[5] ); lua_pushnumber( L, absMatrix[6] ); lua_pushnumber( L, absMatrix[7] );
    lua_pushnumber( L, absMatrix[8] ); lua_pushnumber( L, absMatrix[9] ); lua_pushnumber( L, absMatrix[10] ); lua_pushnumber( L, absMatrix[11] );
    lua_pushnumber( L, absMatrix[12] ); lua_pushnumber( L, absMatrix[13] ); lua_pushnumber( L, absMatrix[14] ); lua_pushnumber( L, absMatrix[15] );

    return 32;
}

static int lua_h3dSetNodeTransMat(lua_State* L)
{
    int node;
    float matrix[16];

    if(lua_gettop(L)!=17)
    {
        return 0;
    }

    node=lua_tonumber(L,1);
    matrix[0]=lua_tonumber(L,2);
    matrix[1]=lua_tonumber(L,3);
    matrix[2]=lua_tonumber(L,4);
    matrix[3]=lua_tonumber(L,5);
    matrix[4]=lua_tonumber(L,6);
    matrix[5]=lua_tonumber(L,7);
    matrix[6]=lua_tonumber(L,8);
    matrix[7]=lua_tonumber(L,9);
    matrix[8]=lua_tonumber(L,10);
    matrix[9]=lua_tonumber(L,11);
    matrix[10]=lua_tonumber(L,12);
    matrix[11]=lua_tonumber(L,13);
    matrix[12]=lua_tonumber(L,14);
    matrix[13]=lua_tonumber(L,15);
    matrix[14]=lua_tonumber(L,16);
    matrix[15]=lua_tonumber(L,17);
    lua_pop(L, 17);

    h3dSetNodeTransMat(node, matrix);

    return 0;
}

static int lua_h3dGetNodeParamF(lua_State* L)
{
    int handle;
    int param, compIdx;
    float result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    param=lua_tonumber(L, 2);
    compIdx = lua_tonumber(L, 3);
    lua_pop(L,3);

    result=h3dGetNodeParamF(handle, param, compIdx);

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetNodeParamF(lua_State* L)
{
    int handle;
    int param, compIdx;
    float value;

    if(lua_gettop(L)!=4)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    param=lua_tonumber(L, 2);
    compIdx = lua_tonumber(L, 3);
    value=lua_tonumber(L, 4);
    lua_pop(L,4);

    h3dSetNodeParamF(handle, param, compIdx, value);
    return 0;
}

static int lua_h3dGetNodeParamI(lua_State* L)
{
    int handle;
    int param;
    int result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    param=lua_tonumber(L, 2);
    lua_pop(L,2);

    result=h3dGetNodeParamI(handle, param);

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetNodeParamI(lua_State* L)
{
    int handle;
    int param;
    int value;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    param=lua_tonumber(L, 2);
    value=lua_tonumber(L, 3);
    lua_pop(L,3);

    h3dSetNodeParamI(handle, param, value);
    return 0;
}

static int lua_h3dGetNodeParamStr(lua_State* L)
{
    int handle;
    int param;
    const char* result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    param=lua_tonumber(L, 2);
    lua_pop(L,2);

    result=h3dGetNodeParamStr(handle, param);

    lua_pushstring(L, result);
    return 1;
}

static int lua_h3dSetNodeParamStr(lua_State* L)
{
    int handle;
    int param;
    const char* value;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    param=lua_tonumber(L, 2);
    value=lua_tostring(L, 3);
    lua_pop(L,3);

    h3dSetNodeParamStr(handle, param, value);
    return 0;
}

static int lua_h3dGetNodeFlags(lua_State* L)
{
    H3DNode node;
    int result;
    if(lua_gettop(L)!=1)
    {
        return 0;
    }
    node = lua_tonumber(L, 1);
    lua_pop(L, 1);
    result = h3dGetNodeFlags(node);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetNodeFlags(lua_State* L)
{
    H3DNode node;
    int flags;
    bool recursive;
    if(lua_gettop(L)!=3)
    {
        return 0;
    }
    node = lua_tonumber(L, 1);
    flags = lua_tointeger(L, 2);
    recursive = lua_toboolean(L, 3);
    lua_pop(L, 3);
    h3dSetNodeFlags(node, flags, recursive);
    return 0;
}

static int lua_h3dGetNodeAABB(lua_State* L)
{
    int node;
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    node=lua_tonumber(L, 1);
    lua_pop(L, 1);

    h3dGetNodeAABB(node, &minX, &minY, &minZ, &maxX, &maxY, &maxZ);

    lua_pushnumber(L, minX);
    lua_pushnumber(L, minY);
    lua_pushnumber(L, minZ);
    lua_pushnumber(L, maxX);
    lua_pushnumber(L, maxY);
    lua_pushnumber(L, maxZ);

    return 6;
}

static int lua_h3dFindNodes(lua_State* L)
{
    int handle;
    const char* name;
    int type;
    int result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    handle=lua_tonumber(L, 1);
    name=lua_tostring(L, 2);
    type=lua_tonumber(L, 3);
    lua_pop(L,3);

    result=h3dFindNodes(handle, name, type);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dGetNodeFindResult(lua_State* L)
{
    int index;
    H3DNode result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    index=lua_tonumber(L, 1);
    lua_pop(L,1);

    result=h3dGetNodeFindResult(index);

    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetNodeUniforms(lua_State* L)
{
    H3DNode node;
    float* uniformData;
    int count;
    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    node=lua_tonumber(L, 1);
    uniformData = (float*)lua_touserdata(L, 2);
    count = lua_tointeger(L, 3);
    lua_pop(L,3);
    h3dSetNodeUniforms(node, uniformData, count);
    return 0;
}

static int lua_h3dCastRay(lua_State* L)
{
    int node;
    float ox, oy, oz;
    float dx, dy, dz;
    int numNearest;
    int result;

    if(lua_gettop(L)!=8)
    {
        return 0;
    }

    node=lua_tonumber(L, 1);
    ox=lua_tonumber(L, 2);
    oy=lua_tonumber(L, 3);
    oz=lua_tonumber(L, 4);
    dx=lua_tonumber(L, 5);
    dy=lua_tonumber(L, 6);
    dz=lua_tonumber(L, 7);
    numNearest=lua_tonumber(L,8);
    lua_pop(L, 8);

    result=h3dCastRay(node, ox, oy, oz, dx, dy, dz, numNearest);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dGetCastRayResult(lua_State* L)
{
    int index;
    int handle;
    float distance;
    float intersection[3];
    bool result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    index=lua_tonumber(L, 1);
    lua_pop(L,1);

    result=h3dGetCastRayResult(index, &handle, &distance, intersection);
    lua_pushnumber(L, handle);
    lua_pushnumber(L, distance);
    lua_pushnumber(L, intersection[0]);
    lua_pushnumber(L, intersection[1]);
    lua_pushnumber(L, intersection[2]);

    return 5;
}

static int lua_h3dCheckNodeVisibility(lua_State* L)
{
    H3DNode node, cameraNode;
    bool checkOcclusion, calcLod;
    int result;
    if(lua_gettop(L)!=4)
    {
        return 0;
    }

    node=lua_tointeger(L, 1);
    cameraNode=lua_tointeger(L, 2);
    checkOcclusion = lua_toboolean(L, 3);
    calcLod = lua_toboolean(L, 4);
    lua_pop(L,4);
    result = h3dCheckNodeVisibility( node, cameraNode, checkOcclusion, calcLod);
    lua_pushinteger(L, result);
    return 1;
}

static int lua_h3dAddGroupNode(lua_State* L)
{
    int parent;
    const char* name;
    int result;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    parent=lua_tonumber(L,1);
    name=lua_tostring(L,2);
    lua_pop(L,2);

    result=h3dAddGroupNode(parent, name);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dAddModelNode(lua_State* L)
{
    int parent;
    const char* name;
    int geometryHandle;
    int result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    parent=lua_tonumber(L,1);
    name=lua_tostring(L,2);
    geometryHandle=lua_tonumber(L, 3);
    lua_pop(L,3);

    result=h3dAddModelNode(parent, name, geometryHandle);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetupModelAnimStage(lua_State* L)
{
    int modelHandle;
    int stage;
    int animationHandle;
    int layer;
    const char* startNode;
    bool additive;

    if(lua_gettop(L)!=6)
    {
        return 0;
    }

    modelHandle=lua_tonumber(L, 1);
    stage=lua_tonumber(L, 2);
    animationHandle=lua_tonumber(L, 3);
    layer = lua_tointeger(L, 4);
    startNode=lua_tostring(L,5);
    additive=lua_toboolean(L, 6);
    lua_pop(L, 6);

    h3dSetupModelAnimStage(modelHandle, stage, animationHandle, layer, startNode, additive);
    return 0;
}

static int lua_h3dSetModelAnimParams(lua_State* L)
{
    int modelHandle;
    int stage;
    float time;
    float weight;

    if(lua_gettop(L)!=4)
    {
        return 0;
    }

    modelHandle=lua_tonumber(L, 1);
    stage=lua_tonumber(L, 2);
    time=lua_tonumber(L, 3);
    weight=lua_tonumber(L,4);
    lua_pop(L, 4);

    h3dSetModelAnimParams(modelHandle, stage, time, weight);
    return 0;
}

static int lua_h3dSetModelMorpher(lua_State* L)
{
    int modelHandle;
    const char* target;
    float weight;
    bool result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    modelHandle=lua_tonumber(L, 1);
    target=lua_tostring(L, 2);
    weight=lua_tonumber(L, 3);
    lua_pop(L, 3);

    result=h3dSetModelMorpher(modelHandle, target, weight);
    lua_pushboolean(L, result);
    return 1;
}

static int lua_h3dUpdateModel(lua_State* L)
{
    H3DNode modelNode;
    int flags;
    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    modelNode=lua_tointeger(L, 1);
    flags=lua_tointeger(L, 2);
    lua_pop(L, 2);
    h3dUpdateModel(modelNode, flags);
    return 0;
}

static int lua_h3dAddMeshNode(lua_State* L)
{
    int parent;
    const char* name;
    int materialHandle;
    int batchStart;
    int batchCount;
    int vertRStart;
    int vertREnd;
    H3DNode result;

    if(lua_gettop(L)!=7)
    {
        return 0;
    }

    parent=lua_tonumber(L, 1);
    name=lua_tostring(L, 2);
    materialHandle=lua_tonumber(L, 3);
    batchStart=lua_tonumber(L,4);
    batchCount=lua_tonumber(L, 5);
    vertRStart=lua_tonumber(L, 6);
    vertREnd=lua_tonumber(L, 7);
    lua_pop(L, 7);

    result=h3dAddMeshNode(parent, name, materialHandle, batchStart, batchCount, vertRStart, vertREnd);
    lua_pushinteger(L, result);
    return 1;
}

static int lua_h3dAddJointNode(lua_State* L)
{
    H3DNode parent;
    const char* name;
    int index;
    H3DNode result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    parent=lua_tonumber(L,1);
    name=lua_tostring(L,2);
    index=lua_tonumber(L, 3);
    lua_pop(L,3);

    result=h3dAddJointNode(parent, name, index);

    lua_pushinteger(L, result);
    return 1;
}

static int lua_h3dAddLightNode(lua_State* L)
{
    H3DNode parent;
    const char* name;
    int materialHandle;
    const char* lightContext;
    const char* shadowContext;

    H3DNode result;

    if(lua_gettop(L)!=5)
    {
        return 0;
    }

    parent=lua_tonumber(L,1);
    name=lua_tostring(L,2);
    materialHandle=lua_tonumber(L, 3);
    lightContext=lua_tostring(L, 4);
    shadowContext=lua_tostring(L, 5);
    lua_pop(L,5);

    result=h3dAddLightNode(parent, name, materialHandle, lightContext, shadowContext);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dAddCameraNode(lua_State* L)
{
    H3DNode parent;
    const char* name;
    int pipelineHandle;
    H3DNode result;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    parent=lua_tonumber(L,1);
    name=lua_tostring(L,2);
    pipelineHandle=lua_tonumber(L, 3);
    lua_pop(L,3);

    result=h3dAddCameraNode(parent, name, pipelineHandle);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dSetupCameraView(lua_State* L)
{
    int camera;
    float fov;
    float aspect;
    float nearDist;
    float farDist;

    if(lua_gettop(L)!=5)
    {
        return 0;
    }

    camera=lua_tonumber(L,1);
    fov=lua_tonumber(L,2);
    aspect=lua_tonumber(L,3);
    nearDist=lua_tonumber(L,4);
    farDist=lua_tonumber(L,5);
    lua_pop(L,5);

    h3dSetupCameraView(camera, fov, aspect, nearDist, farDist);
    return 0;
}

static int lua_h3dGetCameraProjMat(lua_State* L)
{
    int camera;
    float matrix[16];

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    camera=lua_tonumber(L, 1);
    lua_pop(L, 1);

    h3dGetCameraProjMat(camera, matrix);

    lua_pushnumber(L,matrix[0]);
    lua_pushnumber(L,matrix[1]);
    lua_pushnumber(L,matrix[2]);
    lua_pushnumber(L,matrix[3]);
    lua_pushnumber(L,matrix[4]);
    lua_pushnumber(L,matrix[5]);
    lua_pushnumber(L,matrix[6]);
    lua_pushnumber(L,matrix[7]);
    lua_pushnumber(L,matrix[8]);
    lua_pushnumber(L,matrix[9]);
    lua_pushnumber(L,matrix[10]);
    lua_pushnumber(L,matrix[11]);
    lua_pushnumber(L,matrix[12]);
    lua_pushnumber(L,matrix[13]);
    lua_pushnumber(L,matrix[14]);
    lua_pushnumber(L,matrix[15]);

    return 16;
}

static int lua_h3dAddEmitterNode(lua_State* L)
{
    H3DNode parent;
    const char* name;
    int materialHandle;
    int effectHandle;
    int maxParticleCount;
    int respawnCount;
    H3DNode result;

    if(lua_gettop(L)!=6)
    {
        return 0;
    }

    parent=lua_tonumber(L,1);
    name=lua_tostring(L,2);
    materialHandle=lua_tonumber(L,3);
    effectHandle=lua_tonumber(L,4);
    maxParticleCount=lua_tonumber(L,5);
    respawnCount=lua_tonumber(L,6);
    lua_pop(L,6);

    result=h3dAddEmitterNode(parent, name, materialHandle,
                          effectHandle, maxParticleCount, respawnCount);
    lua_pushnumber(L, result);
    return 1;
}

static int lua_h3dUpdateEmitter(lua_State* L)
{
    H3DNode emitter;
    float time;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    emitter=lua_tonumber(L,1);
    time=lua_tonumber(L,2);
    lua_pop(L,2);

    h3dUpdateEmitter(emitter, time);
    return 0;
}

static int lua_h3dHasEmitterFinished(lua_State* L)
{
    int handle;
    bool result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    handle=lua_tonumber(L,1);
    lua_pop(L, 1);

    result=h3dHasEmitterFinished(handle);
    lua_pushboolean(L, result);
    return 1;
}

//--------------------------HORDE 3D Util Functions------------------------
static int lua_h3dutFreeMem(lua_State* L)
{
    char* mem;


    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    mem=(char*)lua_touserdata(L,1);
    lua_pop(L, 1);

    h3dutFreeMem(&mem);

    return 0;
}

static int lua_h3dutDumpMessages(lua_State* L)
{
    bool result=h3dutDumpMessages();
    lua_pushboolean(L,result);
    return 1;
}

static int lua_h3dutInitOpenGL(lua_State* L)
{
    int hdc;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    hdc=lua_tonumber(L,1);
    lua_pop(L, 1);

    bool result=h3dutInitOpenGL(hdc);
    lua_pushboolean(L,result);

    return 1;
}

static int lua_h3dutReleaseOpenGL(lua_State* L)
{
    h3dutReleaseOpenGL();
    return 0;
}

static int lua_h3dutSwapBuffers(lua_State* L)
{
    h3dutSwapBuffers();
    return 0;
}

static int lua_h3dutGetResourcePath(lua_State* L)
{
    int type;
    const char* result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    type=lua_tonumber(L, 1);
    lua_pop(L,1);

    result=h3dutGetResourcePath(type);
    lua_pushstring(L, result);

    return 1;
}

static int lua_h3dutSetResourcePath(lua_State* L)
{
    int type;
    const char* path;

    if(lua_gettop(L)!=2)
    {
        return 0;
    }

    type=lua_tonumber(L, 1);
    path=lua_tostring(L, 2);
    lua_pop(L,2);

    h3dutSetResourcePath(type, path);

    return 0;

}

static int lua_h3dutLoadResourcesFromDisk(lua_State* L)
{
    const char* path;
    bool result;

    if(lua_gettop(L)!=1)
    {
        return 0;
    }

    path=lua_tostring(L, 1);
    lua_pop(L,1);

    result=h3dutLoadResourcesFromDisk(path);
    lua_pushboolean(L, result);

    return 1;
}

static int lua_h3dutCreateGeometryRes(lua_State* L)
{
    // todo
    return 0;
}

static int lua_h3dutCreateTGAImage(lua_State* L)
{
    // todo
    return 0;
}

static int lua_h3dutScreenshot(lua_State* L)
{
    return 0;
}

static int lua_h3dutPickRay(lua_State* L)
{
    int camera;
    float nwx, nwy;
    float ox, oy, oz;
    float dx, dy, dz;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    camera=lua_tonumber(L, 1);
    nwx=lua_tonumber(L, 2);
    nwy=lua_tonumber(L,3);
    lua_pop(L,3);

    h3dutPickRay(camera, nwx, nwy, &ox, &oy, &oz, &dx, &dy, &dz);

    lua_pushnumber(L, ox);
    lua_pushnumber(L, oy);
    lua_pushnumber(L, oz);
    lua_pushnumber(L, dx);
    lua_pushnumber(L, dy);
    lua_pushnumber(L, dz);
    return 6;
}

static int lua_h3dutPickNode(lua_State* L)
{
    int camera;
    float nwx, nwy;
    int handle;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    camera=lua_tonumber(L, 1);
    nwx=lua_tonumber(L, 2);
    nwy=lua_tonumber(L,3);
    lua_pop(L,3);

    handle=h3dutPickNode(camera, nwx, nwy);
    lua_pushnumber(L, handle);
    return 1;
}

static int lua_h3dutShowText(lua_State* L)
{
    const char* text;
    float x, y, size, colR, colG, colB;
    int fontHandle;

    if(lua_gettop(L)!=8)
    {
        return 0;
    }

    text=lua_tostring(L, 1);
    x=lua_tonumber(L, 2);
    y=lua_tonumber(L,3);
    size=lua_tonumber(L,4);
    colR=lua_tonumber(L,5);
    colG=lua_tonumber(L,6);
    colB=lua_tonumber(L,7);
    fontHandle=lua_tonumber(L,8);
    lua_pop(L,8);

    h3dutShowText(text, x, y, size, colR,colG, colB, fontHandle);
    return 0;
}

static int lua_h3dutShowFrameStats(lua_State* L)
{
    H3DRes fontHandle, panelHandle;
    int mode;

    if(lua_gettop(L)!=3)
    {
        return 0;
    }

    fontHandle=lua_tointeger(L,1);
    panelHandle=lua_tointeger(L,2);
    mode=lua_tointeger(L,3);
    lua_pop(L,3);

    h3dutShowFrameStats(fontHandle, panelHandle, mode);

    return 0;
}

//-------------------------------------------------------
static const struct luaL_Reg horde3dLib [] =
{
    {"GetVersionString", lua_h3dGetVersionString},
    {"CheckExtension", lua_h3dCheckExtension},
    {"GetError", lua_h3dGetError},
    {"Init", lua_h3dInit},
    {"Release", lua_h3dRelease},
    {"Render", lua_h3dRender},
    {"FinalizeFrame", lua_h3dFinalizeFrame},
    {"Clear", lua_h3dClear},
    {"GetMessage", lua_h3dGetMessage},
    {"GetOption", lua_h3dGetOption},
    {"SetOption", lua_h3dSetOption},
    {"GetStat", lua_h3dGetStat},
    {"ShowOverlays", lua_h3dShowOverlays},
    {"ClearOverlays", lua_h3dClearOverlays},
    {"GetResType", lua_h3dGetResType},
    {"GetResName", lua_h3dGetResName},
    {"GetNextResource", lua_h3dGetNextResource},
    {"FindResource", lua_h3dFindResource},
    {"AddResource", lua_h3dAddResource},
    {"CloneResource", lua_h3dCloneResource},
    {"RemoveResource", lua_h3dRemoveResource},
    {"IsResLoaded", lua_h3dIsResLoaded},
    {"LoadResource", lua_h3dLoadResource},
    {"UnloadResource", lua_h3dUnloadResource},
    {"GetResElemCount", lua_h3dGetResElemCount},
    {"FindResElem", lua_h3dFindResElem},
    {"GetResParamI", lua_h3dGetResParamI},
    {"SetResParamI", lua_h3dSetResParamI},
    {"GetResParamF", lua_h3dGetResParamF},
    {"SetResParamF", lua_h3dSetResParamF},
    {"GetResParamStr", lua_h3dGetResParamStr},
    {"SetResParamStr", lua_h3dSetResParamStr},
    {"MapResStream", lua_h3dMapResStream},
    {"UnmapResStream", lua_h3dUnmapResStream},
    {"QueryUnloadedResource", lua_h3dQueryUnloadedResource},
    {"ReleaseUnusedResources", lua_h3dReleaseUnusedResources},
    {"CreateTexture", lua_h3dCreateTexture},
    {"SetShaderPreambles", lua_h3dSetShaderPreambles},
    {"SetMaterialUniform", lua_h3dSetMaterialUniform},
    {"ResizePipelineBuffers", lua_h3dResizePipelineBuffers},
    {"GetRenderTargetData", lua_h3dGetRenderTargetData},
    {"GetNodeType", lua_h3dGetNodeType},
    {"GetNodeParent", lua_h3dGetNodeParent},
    {"SetNodeParent", lua_h3dSetNodeParent},
    {"GetNodeChild", lua_h3dGetNodeChild},
    {"AddNodes", lua_h3dAddNodes},
    {"RemoveNode", lua_h3dRemoveNode},
    {"CheckNodeTransFlag", lua_h3dCheckNodeTransFlag},
    {"GetNodeTransform", lua_h3dGetNodeTransform},
    {"SetNodeTransform", lua_h3dSetNodeTransform},
    {"GetNodeTransMats", lua_h3dGetNodeTransMats},
    {"SetNodeTransMat", lua_h3dSetNodeTransMat},
    {"GetNodeParamF", lua_h3dGetNodeParamF},
    {"SetNodeParamF", lua_h3dSetNodeParamF},
    {"GetNodeParamI", lua_h3dGetNodeParamI},
    {"SetNodeParamI", lua_h3dSetNodeParamI},
    {"GetNodeParamStr", lua_h3dGetNodeParamStr},
    {"SetNodeParamStr", lua_h3dSetNodeParamStr},
    {"GetNodeFlags", lua_h3dGetNodeFlags},
    {"SetNodeFlags", lua_h3dSetNodeFlags},
    {"GetNodeAABB", lua_h3dGetNodeAABB},
    {"FindNodes", lua_h3dFindNodes},
    {"GetNodeFindResult", lua_h3dGetNodeFindResult},
    {"SetNodeUniforms", lua_h3dSetNodeUniforms},
    {"CastRay", lua_h3dCastRay},
    {"GetCastRayResult", lua_h3dGetCastRayResult},
    {"CheckNodeVisibility", lua_h3dCheckNodeVisibility},
    {"AddGroupNode", lua_h3dAddGroupNode},
    {"AddModelNode", lua_h3dAddModelNode},
    {"SetupModelAnimStage", lua_h3dSetupModelAnimStage},
    {"SetModelAnimParams", lua_h3dSetModelAnimParams},
    {"SetModelMorpher", lua_h3dSetModelMorpher},
    {"UpdateModel", lua_h3dUpdateModel},
    {"AddMeshNode", lua_h3dAddMeshNode},
    {"AddJointNode", lua_h3dAddJointNode},
    {"AddLightNode", lua_h3dAddLightNode},
    {"AddCameraNode", lua_h3dAddCameraNode},
    {"SetupCameraView", lua_h3dSetupCameraView},
    {"GetCameraProjMat", lua_h3dGetCameraProjMat},
    {"AddEmitterNode", lua_h3dAddEmitterNode},
    {"UpdateEmitter", lua_h3dUpdateEmitter},
    {"HasEmitterFinished", lua_h3dHasEmitterFinished},
    {NULL, NULL} //sentinel
};

static const struct luaL_Reg horde3dUtilLib [] =
{
    //Horde 3D utility
    {"FreeMem", lua_h3dutFreeMem},
    {"DumpMessages", lua_h3dutDumpMessages},
    {"InitOpenGL", lua_h3dutInitOpenGL},
    {"ReleaseOpenGL", lua_h3dutReleaseOpenGL},
    {"SwapBuffers", lua_h3dutSwapBuffers},
    {"GetResourcePath", lua_h3dutGetResourcePath},
    {"SetResourcePath", lua_h3dutSetResourcePath},
    {"LoadResourcesFromDisk", lua_h3dutLoadResourcesFromDisk},
    {"CreateGeometryRes", lua_h3dutCreateGeometryRes},
    {"CreateTGAImage", lua_h3dutCreateTGAImage},
    {"Screenshot", lua_h3dutScreenshot},
    {"PickRay", lua_h3dutPickRay},
    {"PickNode", lua_h3dutPickNode},
    {"ShowText", lua_h3dutShowText},
    {"ShowFrameStats", lua_h3dutShowFrameStats},
    {NULL, NULL} //sentinel
};

int luaopen_Horde3d(lua_State* L)
{
    //create the library table
    luaL_register(L, "h3d", horde3dLib);
    lua_pushnumber(L, 1);
    lua_setfield(L, -2, "RootNode");
    lua_pop(L,1);

    //create the utility table
    luaL_register(L, "h3du", horde3dUtilLib);
    lua_pop(L,1);

    //create the enums tables
    addH3DRenderersConsts(L);
    addH3DOptionsConsts(L);
    addH3DStatsConsts(L);
    addH3DResTypesConsts(L);
    addH3DResFlagsConsts(L);
    addH3DFormatsConsts(L);
    addH3DGeoResConsts(L);
    addH3DAnimResConsts(L);
    addH3DMatResConsts(L);
    addH3DShaderResConsts(L);
    addH3DTexResConsts(L);
    addH3DPartEffResConsts(L);
    addH3DPipeResConsts(L);
    addH3DNodeTypesConsts(L);
    addH3DNodeFlagsConsts(L);
    addH3DNodeParamsConsts(L);
    addH3DModelConsts(L);
    addH3DMeshConsts(L);
    addH3DJointConsts(L);
    addH3DLightConsts(L);
    addH3DCameraConsts(L);
    addH3DEmitterConsts(L);
    addH3DModelUpdateFlagsConsts(L);

    return 0;
}

