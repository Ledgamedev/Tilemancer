function init()
    setName("Blend")
    setDesc("Blends textures")
    setSize(100, 24+64+8+8+18+18+18+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
    addInput("Texture", 24+64+8+8+18)
    addParameter("Mode", "Blend mode\n0 - Add\n1 - Subtract\n2 - Multiply\n3 - Divide\n4 - Min\n5 - Max", 24+64+8+8+18+18, 0, 0, 5)
    addInputParameter("Factor", "Blend factor", 24+64+8+8+18+18+18, 100, 0, 100, true)
end

function apply()
    tileSize = getTileSize()
    mode = getValue(2, 0, 0, 1)
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        factor = getValue(3, x, y, 100.0)
        ar, ag, ab = getValue(0, x, y, 1)
        br, bg, bb = getValue(1, x, y, 1)
        if mode==0 then --add
            fr = ar+br*factor
            fg = ag+bg*factor
            fb = ab+bb*factor
            setPixel(0, x, y, fr, fg, fb)
        elseif mode==1 then --subtract
            fr = ar-br*factor
            fg = ag-bg*factor
            fb = ab-bb*factor
            setPixel(0, x, y, fr, fg, fb)
        elseif mode==2 then --multiply
            fr = ar*(br*factor+(1.0-factor))
            fg = ag*(bg*factor+(1.0-factor))
            fb = ab*(bb*factor+(1.0-factor))
            setPixel(0, x, y, fr, fg, fb)
        elseif mode==3 then --divide
            fr = ar/(br*factor+(1.0-factor))
            fg = ag/(bg*factor+(1.0-factor))
            fb = ab/(bb*factor+(1.0-factor))
            setPixel(0, x, y, fr, fg, fb)
        elseif mode==4 then --min
            fr = math.min(ar, br*factor)
            fg = math.min(ag, bg*factor)
            fb = math.min(ab, bb*factor)
            setPixel(0, x, y, fr, fg, fb)
        elseif mode==5 then --max
            fr = math.max(ar, br*factor)
            fg = math.max(ag, bg*factor)
            fb = math.max(ab, bb*factor)
            setPixel(0, x, y, fr, fg, fb)
        end
    end
end