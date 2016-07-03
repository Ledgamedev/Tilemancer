function init()
    setName("Scatter")
    setDesc("Scatters texture")
    setSize(100, 24+64+8+8+18+18+18+18+18+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
    addParameter("Seed", "RNG seed", 24+64+8+8+18, 0, -1, -1)
    addParameter("Size", "Grid's size", 24+64+8+8+18+18, 2, 2, -1)
    addParameter("Scale", "Random scale", 24+64+8+8+18+18+18, 100, 0, 100, true)
    addParameter("Rotation", "Random rotation", 24+64+8+8+18+18+18+18, 100, 0, 100, true)
    addParameter("Intensity", "Random intensity", 24+64+8+8+18+18+18+18+18, 100, 0, 100, true)
end

function apply()
    math.randomseed(getValue(1, 0, 0, 1))
    gridSize = getValue(2, 0, 0, 1)
    tileSize = getTileSize()
    finalr = { }
    finalg = { }
    finalb = { }
    for i=0, tileSize*tileSize-1 do
        finalr[i] = -math.huge
        finalg[i] = -math.huge
        finalb[i] = -math.huge
    end
    for g=0, gridSize*gridSize-1 do
        x = g%gridSize
        y = math.floor(g/gridSize)
        px = (math.random()+x)*(tileSize/gridSize)
        py = (math.random()+y)*(tileSize/gridSize)
        scalex = tileSize/gridSize*(math.random()*getValue(3, 0, 0, 200.0)+1.0)
        scaley = tileSize/gridSize*(math.random()*getValue(3, 0, 0, 200.0)+1.0)
        rotation = (math.random(0, 360)*getValue(4, 0, 0, 100.0))*math.pi/180.0
        offset = math.random()*getValue(5, 0, 0, 100.0)
        s = math.sin(rotation)
        c = math.cos(rotation)
        for t=0, tileSize*tileSize-1 do
            tx = t%tileSize
            ty = math.floor(t/tileSize)
            for w=0, 8 do
                gx = (w%3)-1
                gy = (math.floor(w/3))-1
                pixelx = (tx-(px+tileSize*gx))
                pixely = (ty-(py+tileSize*gy))
                pixelx2 = pixelx*c-pixely*s
                pixely2 = pixelx*s+pixely*c
                pixelx = pixelx2*tileSize/scalex+tileSize/2.0
                pixely = pixely2*tileSize/scaley+tileSize/2.0
                cr, cg, cb = getValue(0, pixelx, pixely, 1.0)
                if pixelx>=tileSize or pixelx<0 or pixely>=tileSize or pixely<0 then
                    cr = 0
                    cg = 0
                    cb = 0
                end
                cr = cr-offset
                cg = cg-offset
                cb = cb-offset
                finalr[t] = math.max(finalr[t], cr)
                finalg[t] = math.max(finalg[t], cg)
                finalb[t] = math.max(finalb[t], cb)
            end
        end
    end
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        setPixel(0, x, y, finalr[i], finalg[i], finalb[i])
    end
end