function init()
    setName("Cells")
    setDesc("Generates cells")
    setSize(120, 24+64+8+8+18+18+18+7+4)
    addOutput(24+32)
    addParameter("Seed", "RNG seed", 24+64+8+8, 0, -1, -1)
    addParameter("Size", "Grid's size", 24+64+8+8+18, 2, 2, -1)
    addParameter("Falloff", "Border falloff", 24+64+8+8+18+18, 25, 0, 100, true)
    addParameter("Intensity", "Height intensity", 24+64+8+8+18+18+18, 100, 0, -1, true)
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
    gridSize = getValue(1, 0, 0, 1)
    tileSize = getTileSize()
    pointsX = { }
    pointsY = { }
    for i=0, gridSize*gridSize-1 do
        pointsX[i] = math.random()
        pointsY[i] = math.random()
    end
    for p=0, tileSize*tileSize-1 do
        x = p%tileSize
        y = math.floor(p/tileSize)
        nx = math.floor(x/(tileSize/gridSize))
        ny = math.floor(y/(tileSize/gridSize))
        fx = x/(tileSize/gridSize)-nx
        fy = y/(tileSize/gridSize)-ny
        md = 100.0
        mrx, mry = 0
        mgx, mgy = 0
        for i=-1, 1 do
            for j=-1, 1 do
                finalX = nx+i
                while finalX>=gridSize do finalX=finalX-gridSize end
                while finalX<0 do finalX=finalX+gridSize end
                finalY = ny+j
                while finalY>=gridSize do finalY=finalY-gridSize end
                while finalY<0 do finalY=finalY+gridSize end
                index = finalY*gridSize+finalX
                dx = i+pointsX[index]-fx
                dy = j+pointsY[index]-fy
                d = dot(dx, dy, 0, dx, dy, 0)
                if d<md then
                    md = d
                    mrx = dx
                    mry = dy
                    mgx = i
                    mgy = j
                end
            end
        end
        md = 100.0
        for i=-2, 2 do
            for j=-2, 2 do
                finalX = nx+i+mgx
                while finalX>=gridSize do finalX=finalX-gridSize end
                while finalX<0 do finalX=finalX+gridSize end
                finalY = ny+j+mgy
                while finalY>=gridSize do finalY=finalY-gridSize end
                while finalY<0 do finalY=finalY+gridSize end
                index = finalY*gridSize+finalX
                dx = i+mgx+pointsX[index]-fx
                dy = j+mgy+pointsY[index]-fy
                if dot(mrx-dx, mry-dy, 0, mrx-dx, mry-dy, 0)>0.00001 then
                    md = math.min(md, dot(0.5*(mrx+dx), 0.5*(mry+dy), 0, normalize(dx-mrx, dy-mry, 0)))
                end
            end
        end
        falloff = getValue(2, 0, 0, 100.0)
        intensity = getValue(3, 0, 0, 100.0)
        md = math.max(0.0, math.min(1.0, md/falloff))*intensity
        setPixel(0, x, y, md, md, md)
    end
end