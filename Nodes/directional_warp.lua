function init()
    setName("Directional Warp")
    setDesc("Warps texture in a direction")
    setSize(140, 24+64+8+8+18+18+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
    addInputParameter("Direction", "Blur direction", 24+64+8+8+18, 0, -1, -1)
    addInputParameter("Intensity", "Blur intensity", 24+64+8+8+18+18, 10, 0, 100, true)
end

function normalize(x, y, z)
    l = math.sqrt(x*x+y*y+z*z)
    return x/l, y/l, z/l
end

function apply()
    tileSize = getTileSize()
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        intensity = getValue(2, x, y, 100.0)
        dist = tileSize*intensity
        direction = getValue(1, x, y, 360.0)*360.0*math.pi/180.0
        dx, dy = normalize(math.sin(direction), -math.cos(direction), 0.0)
        cr, cg, cb = getValue(0, x+dx*dist, y+dy*dist, 1.0)
        fr = cr
        fg = cg
        fb = cb
        setPixel(0, x, y, fr, fg, fb)
    end
end