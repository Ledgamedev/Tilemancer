function init()
    setName("Colorizer")
    setDesc("Colors texture")
    setSize(108, 24+64+8+4+18+18+18+18+7+4)
    addOutput(24+32)
    addInput("Texture", 24+64+8+8)
    addCRamp(24+64+8+18+4)
end

function apply()
    colorize()
end