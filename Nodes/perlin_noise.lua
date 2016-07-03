function init()
    setName("Perlin Noise")
    setDesc("Generates Perlin noise")
    setSize(120, 24+64+8+8+18+18+18+18+18+7+4)
    addOutput(24+32)
    addParameter("Seed", "RNG seed", 24+64+8+8, 0, -1, -1)
    addParameter("Size", "Grid's size", 24+64+8+8+18, 2, 2, -1)
    addParameter("Layers", "Number of layers", 24+64+8+8+18+18, 3, 1, -1)
    addParameter("Lacunarity", "Size between layers", 24+64+8+8+18+18+18, 200, 100, -1, true)
    addParameter("Gain", "Intensity between layers", 24+64+8+8+18+18+18+18, 80, 0, 100, true)
    addParameter("Intensity", "Height intensity", 24+64+8+8+18+18+18+18+18, 100, 0, -1, true)
end

function normalize(x, y, z)
    l = math.sqrt(x*x+y*y+z*z)
    if l==0 then
        return 0, 0, 0
    else
        return x/l, y/l, z/l
    end
end

function dot(ax, ay, az, bx, by, bz)
    return ax*bx+ay*by+az*bz
end

function apply()
    math.randomseed(getValue(0, 0, 0, 1))
    layers = getValue(2, 0, 0, 1)
    lacunarity = getValue(3, 0, 0, 1)
    gain = getValue(4, 0, 0, 1)
    tileSize = getTileSize()
    finalValues = { }
    for ll=0, layers-1 do
        gridSizeX = math.floor(getValue(1, 0, 0, 1)*math.pow(lacunarity/100.0, ll))
        gridSizeY = math.floor(getValue(1, 0, 0, 1)*math.pow(lacunarity/100.0, ll))
        if gridSizeX<2 then gridSizeX=2 end
        if gridSizeY<2 then gridSizeY=2 end
        vectorX = { }
        vectorY = { }
        for x=0, gridSizeX-1 do
            for y=0, gridSizeX-1 do
                index = y*gridSizeX+x
                vx = math.random(-100, 100)/100.0
                vy = math.random(-100, 100)/100.0
                vx, vy = normalize(vx, vy, 0)
                if vx==0 and vy==0 then
                    vx = 1.0
                    vy = 0.0
                end
                vectorX[index] = vx
                vectorY[index] = vy
            end
        end
        for y=0, gridSizeY-1 do
            indexA = y*gridSizeX
            indexB = y*gridSizeX+gridSizeX-1
            vectorX[indexB] = vectorX[indexA]
            vectorY[indexB] = vectorY[indexA]
        end
        for x=0, gridSizeX-1 do
            indexA = x
            indexB = (gridSizeY-1)*gridSizeX+x
            vectorX[indexB] = vectorX[indexA]
            vectorY[indexB] = vectorY[indexA]
        end
        for i=0, tileSize*tileSize-1 do
            x = i%tileSize
            y = math.floor(i/tileSize)

            gridX = math.floor(x/(tileSize/(gridSizeX-1)))
            gridY = math.floor(y/(tileSize/(gridSizeY-1)))

            d0 = 0
            d1 = 0
            d2 = 0
            d3 = 0
            for nx=0, 1 do
                for ny=0, 1 do
                    dx = (x-(gridX+nx)*(tileSize/(gridSizeX-1)))/(tileSize/(gridSizeX-1))
                    dy = (y-(gridY+ny)*(tileSize/(gridSizeY-1)))/(tileSize/(gridSizeY-1))
                    index = (gridY+ny)*gridSizeX+gridX+nx
                    vx = vectorX[index]
                    vy = vectorY[index]
                    if nx==0 and ny==0 then
                        d0 = dot(dx, dy, 0, vx, vy, 0)
                    elseif nx==0 and ny==1 then
                        d1 = dot(dx, dy, 0, vx, vy, 0)
                    elseif nx==1 and ny==0 then
                        d2 = dot(dx, dy, 0, vx, vy, 0)
                    elseif nx==1 and ny==1 then
                        d3 = dot(dx, dy, 0, vx, vy, 0)
                    end
                end
            end
            alphaX = (x-gridX*(tileSize/(gridSizeX-1)))/(tileSize/(gridSizeX-1))
            alphaY = (y-gridY*(tileSize/(gridSizeY-1)))/(tileSize/(gridSizeY-1))
            alphaX = alphaX*alphaX*alphaX*(alphaX*(alphaX*6-15)+10)
            alphaY = alphaY*alphaY*alphaY*(alphaY*(alphaY*6-15)+10)
            top = d0*(1.0-alphaX)+d2*alphaX
            bottom = d1*(1.0-alphaX)+d3*alphaX
            center = top*(1.0-alphaY)+bottom*alphaY
            center = (center+1.0)/2.0
            if ll==0 then
                finalValues[i] = center
            else
                alpha = math.pow(gain/100.0, ll)
                old = finalValues[i]
                center = center*alpha+old*(1.0-alpha);
                finalValues[i] = old*(old+(2*center)*(1.0-old));
            end
        end
    end
    intensity = getValue(5, 0, 0, 100.0)
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)

        value = finalValues[i]*intensity
        setPixel(0, x, y, value, value, value)
    end
end