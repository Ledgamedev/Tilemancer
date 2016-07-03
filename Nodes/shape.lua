function init()
    setName("Shape")
    setDesc("Generates a shape")
    setSize(100, 24+64+8+8+18+7+4)
    addOutput(24+32)
    addParameter("Type", "Shape type", 24+64+8+8, 0, 0, 10)
    addParameter("Intensity", "Height intensity", 24+64+8+8+18, 100, 0, -1, true)
end

function apply()
    tileSize = getTileSize()
    mode = getValue(0, 0, 0, 1)
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        value = 0
        if mode==0 then
            dx = x-tileSize/2.0
            dy = y-tileSize/2.0
            d = math.sqrt(dx*dx+dy*dy)
            value = math.min(1.0, (d/(tileSize/2.0)))
            value = 1.0-value*value
        end
        if mode==1 then
            dx = x-tileSize/2.0
            dy = y-tileSize/2.0
            d = math.abs(dx)+math.abs(dy)
            value = 1.0-math.min(1.0, (d/(tileSize/2.0)))
        end
        if mode==2 then
            dx = x-tileSize/2.0
            dy = y-tileSize/2.0
            d = math.max(math.abs(dx), math.abs(dy))
            value = 1.0-math.min(1.0, (d/(tileSize/2.0)))
        end
        if mode==3 then
            value = 1.0-(y/tileSize)
        end
        if mode==4 then
            value = x/tileSize
        end
        if mode==5 then
            value = y/tileSize
        end
        if mode==6 then
            value = 1.0-(x/tileSize)
        end
        --thanks to rerere284 for the following four modes
        if mode==7 then
            dx = x-tileSize/2.0
            dy = y-tileSize/2.0
            rot = math.atan2(dy, dx)*180/math.pi
            if rot<0 then
                rot = rot+360;
            end
            value = rot/360.0
        end
        if mode==8 then
            dx = x-tileSize/2.0
            dy = y-tileSize/2.0
            rot = math.atan2(dy, dx)*180/math.pi-90
            if rot<0 then
                rot = rot+360;
            end
            value = rot/360.0
        end
        if mode==9 then
            dx = x-tileSize/2.0
            dy = y-tileSize/2.0
            rot = math.atan2(dy, dx)*180/math.pi+180
            if rot<0 then
                rot = rot+360;
            end
            value = rot/360.0
        end
        if mode==10 then
            dx = x-tileSize/2.0
            dy = y-tileSize/2.0
            rot = math.atan2(dy, dx)*180/math.pi+90
            if rot<0 then
                rot = rot+360;
            end
            value = rot/360.0
        end
        value = value*getValue(1, 0, 0, 100.0)
        setPixel(0, x, y, value, value, value)
    end
end