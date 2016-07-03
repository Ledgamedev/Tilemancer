--original made by navot
function init()
    setName("Edge Detect")
    setDesc("Highlights edges")
    setSize(120, 24+64+8+8+18+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
    addParameter("Intensity", "Height intensity", 24+64+8+8+18, 100, 0, -1, true)
end

function apply()
    sobelH = {-1, -2, -1, 0, 0, 0, 1, 2, 1}
    sobelV = {-1, 0, 1, -2, 0, 2, -1, 0, 1}
    cDat = { }
    tileSize = getTileSize()
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        cr = 0
        cg = 0
        cb = 0
        cgrayV = 0
        cgrayH = 0

        for k=0, 2 do
            for j=0, 2 do
                t = j+k*3
                tr, tg, tb = getValue(0, x+k-1, y+j-1, 1)
                cgray = (cr+cg+cb)/3*getValue(1, 0, 0, 100)
                tgray = (tr+tg+tb)/3*getValue(1, 0, 0, 100)
                cgrayV = (cgrayV+tgray*sobelV[t+1])
                cgrayH = (cgrayH+tgray*-sobelH[t+1]) 
            end
        end

        cr = math.sqrt(cgrayV*cgrayV+cgrayH*cgrayH)*getValue(1, 0, 0, 100)
        cg = cr
        cb = cr
        setPixel(0, x, y, cr, cg, cb)
    end
end