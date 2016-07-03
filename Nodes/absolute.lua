function init()
    setName("Absolute")
    setDesc("Gets absolute value")
    setSize(100, 24+64+8+8+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
end

function apply()
    tileSize = getTileSize()
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        cr, cg, cb = getValue(0, x, y, 1.0)
        setPixel(0, x, y, math.abs(cr), math.abs(cg), math.abs(cb))
    end
end