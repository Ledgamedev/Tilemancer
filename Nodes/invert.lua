function init()
    setName("Invert")
    setDesc("Inverts texture's colors")
    setSize(80, 24+64+8+8+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
end

function apply()
    tileSize = getTileSize()
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        cr, cg, cb = getValue(0, x, y, 1.0)
        setPixel(0, x, y, 1.0-cr, 1.0-cg, 1.0-cb)
    end
end