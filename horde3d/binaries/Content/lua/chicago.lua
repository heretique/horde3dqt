require("h3d")
require("h3du")

local app = {}
app._contentDir = "e:/horde3dContent/Content"
function onCreate()
	print("onCreate")
	local res = h3d.Init(H3DRenderers.OpenGL_ES_2_0)
	if  res == false then
		h3du.DumpMessages()
		h3d.Release()
		return
	end

    h3d.SetOption( H3DOptions.LoadTextures, 1 );
    h3d.SetOption( H3DOptions.TexCompression, 0 );
    h3d.SetOption( H3DOptions.MaxAnisotropy, 4 );
    h3d.SetOption( H3DOptions.ShadowMapSize, 2048 );
    h3d.SetOption( H3DOptions.FastAnimation, 1 );

    app._forwardPipeRes = h3d.AddResource( H3DResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0 );
    app._fontMatRes = h3d.AddResource( H3DResTypes.Material, "overlays/font.material.xml", 0 );
    app._panelMatRes = h3d.AddResource( H3DResTypes.Material, "overlays/panel.material.xml", 0 );
    local envRes = h3d.AddResource( H3DResTypes.SceneGraph, "models/platform/platform.scene.xml", 0 );
    -- Skybox
    local skyBoxRes = h3d.AddResource( H3DResTypes.SceneGraph, "models/skybox/skybox.scene.xml", 0 );

    -- Load resources
    h3du.LoadResourcesFromDisk( app._contentDir );

    -- Add scene nodes
    -- Add camera
    app._cam = h3d.AddCameraNode( h3d.RootNode, "Camera", app._forwardPipeRes );

    --Add environment
    local env = h3d.AddNodes( h3d.RootNode, envRes );
    h3d.SetNodeTransform( env, 0, 0, 0, 0, 0, 0, 0.23, 0.23, 0.23 );
    -- Add skybox
    local sky = h3d.AddNodes( h3d.RootNode, skyBoxRes );
    h3d.SetNodeTransform( sky, 0, 0, 0, 0, 0, 0, 210, 50, 210 );
    h3d.SetNodeFlags( sky, H3DNodeFlags.NoCastShadow, true );
    -- Add light source
    local light = h3d.AddLightNode( h3d.RootNode, "Light1", 0, "LIGHTING", "SHADOWMAP" );
    h3d.SetNodeTransform( light, 0, 20, 50, -30, 0, 0, 1, 1, 1 );
    h3d.SetNodeParamF( light, H3DLight.RadiusF, 0, 200 );
    h3d.SetNodeParamF( light, H3DLight.FovF, 0, 90 );
    h3d.SetNodeParamI( light, H3DLight.ShadowMapCountI, 3 );
    h3d.SetNodeParamF( light, H3DLight.ShadowSplitLambdaF, 0, 0.9 );
    h3d.SetNodeParamF( light, H3DLight.ShadowMapBiasF, 0, 0.001 );
    h3d.SetNodeParamF( light, H3DLight.ColorF3, 0, 0.9 );
    h3d.SetNodeParamF( light, H3DLight.ColorF3, 1, 0.7 );
    h3d.SetNodeParamF( light, H3DLight.ColorF3, 2, 0.75 );
end

function onFreeEGL()
	print("onFreeEGL")
	h3d.Release()
end

function onUpdate(dt)
	--print("onUpdate")
end

function onDestroy()
	print("onDestroy")
end

function onSizeChanged(width, height)
	print("onSizeChanged")
	-- Resize viewport
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportXI, 0 );
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportYI, 0 );
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportWidthI, width );
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportHeightI, height );

    -- Set virtual camera parameters
    h3d.SetupCameraView( app._cam, 45.0, width / height, 0.1, 1000.0 );
    h3d.ResizePipelineBuffers( app._forwardPipeRes, width, height );
end


function onRender()
	--print("onRender")
	-- Render scene
	h3d.SetNodeTransform( app._cam, 15, 3, 20, -10 ,60, 0, 1, 1, 1 );
	h3du.ShowFrameStats( app._fontMatRes, app._panelMatRes, 1 );
	h3du.ShowText( "Pipeline: forward", 0.03, 0.24, 0.026, 1, 1, 1, app._fontMatRes );
    h3d.Render( app._cam );

    -- Finish rendering of frame
    h3d.FinalizeFrame();

    -- Remove all overlays
    h3d.ClearOverlays();
end