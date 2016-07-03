function init()
    setName("Bricks")
    setDesc("Generates bricks")
    setSize(100, 24+64+8+8+18+18+18+18+18+7+4)
    addOutput(24+32)
    addParameter("Seed", "RNG seed", 24+64+8+8, 0, -1, -1)
    addParameter("Size", "Grid's size", 24+64+8+8+18, 1, 1, -1)
    addParameter("Max W", "Maximum width", 24+64+8+8+18+18, 1, 1, -1)
    addParameter("Max H", "Maximum height", 24+64+8+8+18+18+18, 1, 1, -1)
    addParameter("Falloff", "Border falloff", 24+64+8+8+18+18+18+18, 50, 0, 100, true)
    addParameter("Intensity", "Height intensity", 24+64+8+8+18+18+18+18+18, 100, 0, -1, true)
end

function apply()
    math.randomseed(getValue(0, 0, 0, 1))
    gridSize = getValue(1, 0, 0, 1)
    minX = 1
    minY = 1
    maxX = getValue(2, 0, 0, 1)
    maxY = getValue(3, 0, 0, 1)
    minX = math.min(maxX, minX)
    minY = math.min(maxY, minY)
    tileSize = getTileSize()
    grid = { }
    done = false
    while done==false do
        gridX = 0
        gridY = 0
        invalid = true
        while invalid==true do
            invalid = false
            gridX = math.random(0, gridSize-1)
            gridY = math.random(0, gridSize-1)
            if grid[i]~=nil then
                invalid = true
            end
        end
        sizeX = math.random(minX, maxX)
        sizeY = math.random(minY, maxY)
        for x=0, sizeX-1 do
            for y=0, sizeY-1 do
                finalX = gridX+x
                while finalX>=gridSize do finalX=finalX-gridSize end
                finalY = gridY+y
                while finalY>=gridSize do finalY=finalY-gridSize end
                index = finalY*gridSize+finalX
                if grid[index]~=nil then
                    if y==0 then
                        if sizeX>=x then sizeX=x end
                        else
                        if sizeY>=y then sizeY=y end
                    end
                end
            end
        end
        
        for x=0, sizeX-1 do
            for y=0, sizeY-1 do
                finalX = gridX+x
                while finalX>=gridSize do finalX=finalX-gridSize end
                finalY = gridY+y
                while finalY>=gridSize do finalY=finalY-gridSize end
                index = finalY*gridSize+finalX
                if grid[index]==nil then
                    value = 0
                    if x==sizeX-1 then value=bit32.replace(value, 1, 1) end
                    if y==sizeY-1 then value=bit32.replace(value, 1, 0) end
                    grid[index] = value
                end
            end
        end
        
        done = true
        for i=0, gridSize*gridSize-1 do
            if grid[i]==nil then
                done = false
            end
        end
    end
    
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        
        gridX = math.floor(x/(tileSize/gridSize))
        gridY = math.floor(y/(tileSize/gridSize))
        
        gridC = gridY*gridSize+gridX
        gridT = gridY-1<0 and (gridSize-1)*gridSize+gridX or (gridY-1)*gridSize+gridX
        gridL = gridX-1<0 and gridY*gridSize+gridSize-1 or gridY*gridSize+gridX-1
        
        dist = 0
        if math.min(x-math.floor(gridX*(tileSize/gridSize)), math.floor((gridX+1)*(tileSize/gridSize))-x)<math.min(y-math.floor(gridY*(tileSize/gridSize)), math.floor((gridY+1)*(tileSize/gridSize))-y) then
            if x < math.floor((gridX+0.5)*(tileSize/gridSize)) then
                if bit32.extract(grid[gridL], 1)==0 then
                    dist = tileSize/gridSize
                    if y < math.floor((gridY+0.5)*(tileSize/gridSize)) then
                        if bit32.extract(grid[gridT], 0)==1 then
                            dist = y-math.floor(gridY*(tileSize/gridSize))
                        end
                        else
                        if bit32.extract(grid[gridC], 0)==1 then
                            dist = math.floor((gridY+1)*(tileSize/gridSize))-y
                        end
                    end
                    else
                    dist = x-math.floor(gridX*(tileSize/gridSize))
                end
                else
                if bit32.extract(grid[gridC], 1)==0 then
                    dist = tileSize/gridSize
                    if y < math.floor((gridY+0.5)*(tileSize/gridSize)) then
                        if bit32.extract(grid[gridT], 0)==1 then
                            dist = y-math.floor(gridY*(tileSize/gridSize))
                        end
                        else
                        if bit32.extract(grid[gridC], 0)==1 then
                            dist = math.floor((gridY+1)*(tileSize/gridSize))-y
                        end
                    end
                    else
                    dist = math.floor((gridX+1)*(tileSize/gridSize))-x
                end
            end
            else
            if y < math.floor((gridY+0.5)*(tileSize/gridSize)) then
                if bit32.extract(grid[gridT], 0)==0 then
                    dist = tileSize/gridSize
                    if x < math.floor((gridX+0.5)*(tileSize/gridSize)) then
                        if bit32.extract(grid[gridL], 1)==1 then
                            dist = x-math.floor(gridX*(tileSize/gridSize))
                        end
                        else
                        if bit32.extract(grid[gridC], 1)==1 then
                            dist = math.floor((gridX+1)*(tileSize/gridSize))-x
                        end
                    end
                    else
                    dist = y-math.floor(gridY*(tileSize/gridSize))
                end
                else
                if bit32.extract(grid[gridC], 0)==0 then
                    dist = tileSize/gridSize
                    if x < math.floor((gridX+0.5)*(tileSize/gridSize)) then
                        if bit32.extract(grid[gridL], 1)==1 then
                            dist = x-math.floor(gridX*(tileSize/gridSize))
                        end
                        else
                        if bit32.extract(grid[gridC], 1)==1 then
                            dist = math.floor((gridX+1)*(tileSize/gridSize))-x
                        end
                    end
                    else
                    dist = math.floor((gridY+1)*(tileSize/gridSize))-y
                end
            end
        end
        dist = dist/(tileSize/gridSize/2.0)
        falloff = getValue(4, 0, 0, 100.0)
        intensity = getValue(5, 0, 0, 100.0)
        dist = math.max(0.0, math.min(1.0, dist/falloff))*intensity
        setPixel(0, x, y, dist, dist, dist)
    end
end