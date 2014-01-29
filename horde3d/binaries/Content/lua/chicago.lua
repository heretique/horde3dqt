-- package.path = package.path .. ";c:/apps/ZeroBrane/lualibs/mobdebug/?.lua"
-- require('mobdebug').start()
require("h3d")
require("h3du")

local app = {
	_x = 15, _y = 3, _z = 20, _rx = -10, _ry = 60, _velocity = 10.0, _curFPS = 30,
	_keys = {}
}
app._contentDir = "../Content"

local crowd = {
	
}

function degToRad( f ) 
	return f * (3.1415926 / 180.0);
end


function crowd.Init()
	print "Crowd Init"
end

function onCreate()
	print("onCreate")

	local res = h3d.Init(H3DRenderers.OpenGL_ES_2_0)
	if  res == false then
		h3du.DumpMessages()
		h3d.Release()
		return
	end

    h3d.SetOption( H3DOptions.LoadTextures, 1 )
    h3d.SetOption( H3DOptions.TexCompression, 0 )
    h3d.SetOption( H3DOptions.MaxAnisotropy, 4 )
    h3d.SetOption( H3DOptions.ShadowMapSize, 2048 )
    h3d.SetOption( H3DOptions.FastAnimation, 1 )

    app._forwardPipeRes = h3d.AddResource( H3DResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0 )
    app._fontMatRes = h3d.AddResource( H3DResTypes.Material, "overlays/font.material.xml", 0 );
    app._panelMatRes = h3d.AddResource( H3DResTypes.Material, "overlays/panel.material.xml", 0 );
    local envRes = h3d.AddResource( H3DResTypes.SceneGraph, "models/platform/platform.scene.xml", 0 )
    -- Skybox
    local skyBoxRes = h3d.AddResource( H3DResTypes.SceneGraph, "models/skybox/skybox.scene.xml", 0 )
    -- LuaCube
    local luacubeRes = h3d.AddResource(H3DResTypes.SceneGraph, "models/LuaCube/LuaCube.scene.xml", 0)
    local spinyRes = h3d.AddResource(H3DResTypes.SceneGraph, "models/Spiny/spiny.scene.xml", 0)
    -- Load resources
    h3du.LoadResourcesFromDisk( app._contentDir );

    -- Add scene nodes
    -- Add camera
    app._cam = h3d.AddCameraNode( h3d.RootNode, "Camera", app._forwardPipeRes );

    --Add environment
    local env = h3d.AddNodes( h3d.RootNode, envRes )
    h3d.SetNodeTransform( env, 0, 0, 0, 0, 0, 0, 0.23, 0.23, 0.23 )
    -- Add skybox
    local sky = h3d.AddNodes( h3d.RootNode, skyBoxRes )
    h3d.SetNodeTransform( sky, 0, 0, 0, 0, 0, 0, 210, 50, 210 )
    h3d.SetNodeFlags( sky, H3DNodeFlags.NoCastShadow, true )
    -- Add luacube
    luacube = h3d.AddNodes( h3d.RootNode, luacubeRes)
    h3d.SetNodeTransform( luacube, 0, 2.5, 0, 0, 0, 0, 2, 2, 2 )
    luacubeRot = 0
    -- h3d.SetNodeFlags( luacube, H3DNodeFlags.NoCastShadow, true )
    spiny = h3d.AddNodes( h3d.RootNode, spinyRes)
     h3d.SetNodeTransform( spiny, 10, .5, 0, 0, 0, 0, 2, 2, 2 )
    -- Add light source
    local light = h3d.AddLightNode( h3d.RootNode, "Light1", 0, "LIGHTING", "SHADOWMAP" )
    h3d.SetNodeTransform( light, 0, 20, 50, -30, 0, 0, 1, 1, 1 )
    h3d.SetNodeParamF( light, H3DLight.RadiusF, 0, 200 )
    h3d.SetNodeParamF( light, H3DLight.FovF, 0, 90 )
    h3d.SetNodeParamI( light, H3DLight.ShadowMapCountI, 3 )
    h3d.SetNodeParamF( light, H3DLight.ShadowSplitLambdaF, 0, 0.9 )
    h3d.SetNodeParamF( light, H3DLight.ShadowMapBiasF, 0, 0.001 )
    h3d.SetNodeParamF( light, H3DLight.ColorF3, 0, 0.9 )
    h3d.SetNodeParamF( light, H3DLight.ColorF3, 1, 0.7 )
    h3d.SetNodeParamF( light, H3DLight.ColorF3, 2, 0.75 )

    crowd.Init()
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
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportXI, 0 )
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportYI, 0 )
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportWidthI, width )
    h3d.SetNodeParamI( app._cam, H3DCamera.ViewportHeightI, height )

    -- Set virtual camera parameters
    h3d.SetupCameraView( app._cam, 45.0, width / height, 0.1, 1000.0 )
    h3d.ResizePipelineBuffers( app._forwardPipeRes, width, height )
end


function onRender()
	--print("onRender")
	keyHandler()
	-- Render scene
	h3d.SetNodeTransform( app._cam, app._x, app._y, app._z, app._rx ,app._ry, 0, 1, 1, 1 )
	luacubeRot = luacubeRot + 2
    h3d.SetNodeTransform( luacube, 0, 2.5, 0, luacubeRot, luacubeRot, luacubeRot, 2, 2, 2 )
    h3d.SetNodeTransform( spiny, 10, .5, 0, 0, luacubeRot, 0, 2, 2, 2)
	h3du.ShowFrameStats( app._fontMatRes, app._panelMatRes, 1 )
	h3du.ShowText( "Pipeline: forward", 0.03, 0.24, 0.026, 1, 1, 1, app._fontMatRes )
    h3d.Render( app._cam )

    -- Finish rendering of frame
    h3d.FinalizeFrame()

    -- Remove all overlays
    h3d.ClearOverlays()
end


function keyPressEvent(key)
	--print("keyPressEvent: " .. key)
	app._keys[key] = true

end

function keyReleaseEvent(key)
	--print("keyReleaseEvent: " .. key)
	app._keys[key] = nil
end

function mousePressEvent(buttons, posX, poxY)
end

function mouseMoveEvent(dX, dY)
	--print("mouseMoveEvent: (" .. dX .. ", " .. dY .. ")")
	app._ry = app._ry + (dX / 100 * 30)
	
	-- Loop up/down but only in a limited range
	app._rx = app._rx + (dY / 100 * 30)
	if( app._rx > 90 )  then app._rx = 90 end
	if( app._rx < -90 ) then app._rx = -90 end
end

function mouseReleaseEvent(buttons, posX, poxY)
end

function keyHandler()

		local curVel = app._velocity / app._curFPS

		if app._keys[Key.Key_Shift] == true then curVel = curVel * 5	end -- LShift
		
		if app._keys[Key.Key_W] == true then
			-- Move forward
			app._x = app._x - math.sin( degToRad( app._ry ) ) * math.cos( -degToRad( app._rx ) ) * curVel;
			app._y = app._y - math.sin( -degToRad( app._rx ) ) * curVel;
			app._z = app._z - math.cos( degToRad( app._ry ) ) * math.cos( -degToRad( app._rx ) ) * curVel;
		elseif app._keys[Key.Key_S] == true then
			-- Move backward
			app._x = app._x + math.sin( degToRad( app._ry ) ) * math.cos( -degToRad( app._rx ) ) * curVel;
			app._y = app._y + math.sin( -degToRad( app._rx ) ) * curVel;
			app._z = app._z + math.cos( degToRad( app._ry ) ) * math.cos( -degToRad( app._rx ) ) * curVel;
		elseif app._keys[Key.Key_A] then
			-- Strafe left
			app._x = app._x + math.sin( degToRad( app._ry - 90) ) * curVel;
			app._z = app._z + math.cos( degToRad( app._ry - 90 ) ) * curVel;
		elseif app._keys[Key.Key_D] then
			-- Strafe right
			app._x = app._x + math.sin( degToRad( app._ry + 90 ) ) * curVel;
			app._z = app._z + math.cos( degToRad( app._ry + 90 ) ) * curVel;
		end
end