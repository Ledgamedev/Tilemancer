function init()
    setName("Blur")
    setDesc("Blurs texture")
    setSize(100, 24+64+8+8+18+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
    addParameter("Intensity", "Blur intensity", 24+64+8+8+18, 10, 0, 100, true)
end

function apply()
    tileSize = getTileSize()

    intensity = getValue(1, 0, 0, 100.0)
    radius = math.floor(tileSize*intensity)
    kernel = { }
    a = 1.0/(2.0*radius*radius)
    b = 1.0/(math.sqrt(2.0*math.pi)*radius)
    c = 1.0
    r = -radius
    total = 0
    for i=0, radius*2 do
        x = r*r
        kernel[i] = b*math.exp(-x*b)
        total = total+kernel[i]
        r = r+1
    end
    for i=0, radius*2 do
        kernel[i] = kernel[i]/total
        if radius==0 then
            kernel[i] = 1
        end
    end

    blurr = { }
    blurg = { }
    blurb = { }
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        sumr = 0;
        sumg = 0;
        sumb = 0;
        for bx=0, radius*2 do
            cr, cg, cb = getValue(0, x-radius+bx, y, 1.0)
            sumr = sumr+cr*kernel[bx]
            sumg = sumg+cg*kernel[bx]
            sumb = sumb+cb*kernel[bx]
        end
        blurr[i] = sumr
        blurg[i] = sumg
        blurb[i] = sumb
    end
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        sumr = 0;
        sumg = 0;
        sumb = 0;
        for by=0, radius*2 do
            ny = y-radius+by
            while ny<0 do ny=ny+tileSize end
            while ny>=tileSize do ny=ny-tileSize end
            index = ny*tileSize+x
            cr = blurr[index]
            cg = blurg[index]
            cb = blurb[index]
            sumr = sumr+cr*kernel[by]
            sumg = sumg+cg*kernel[by]
            sumb = sumb+cb*kernel[by]
        end
        setPixel(0, x, y, sumr, sumg, sumb)
    end
end