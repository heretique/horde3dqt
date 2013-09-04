require("h3d")
require("h3du")

function onCreate()
	print("onCreate")
	local res = h3d.Init()
	if  res == false then
		h3du.DumpMessages()
	end
end

function onFreeEGL()
	print("onFreeEGL")
end

function onUpdate(dt)
	--print("onUpdate")
end

function onDestroy()
	print("onDestroy")
end

function onSizeChanged(width, height)
	print("onSizeChanged")
end


function onRender()
	--print("onRender")
end