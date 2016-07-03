function init()
    setName("Directional Blur")
    setDesc("Blurs texture in a direction")
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
        samples = math.max(1.0, math.floor(dist))
        direction = getValue(1, x, y, 360.0)*360.0*math.pi/180.0
        dx, dy = normalize(math.sin(direction), -math.cos(direction), 0.0)
        fr = 0
        fg = 0
        fb = 0
        for b=0, samples-1 do
            cr, cg, cb = getValue(0, x+dx*dist*b/samples, y+dy*dist*b/samples, 1.0)
            fr = fr+cr
            fg = fg+cg
            fb = fb+cb
        end
        fr = fr/samples
        fg = fg/samples
        fb = fb/samples
        setPixel(0, x, y, fr, fg, fb)
    end
end