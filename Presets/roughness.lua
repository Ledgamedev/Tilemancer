function init()
    setName("Roughness")
    setDesc("Sets up roughness")
end

function apply()
    math.randomseed(os.time())
    addNode("perlin_noise.lua", -70, 0)
    addNode("blend.lua", 50, 0)
    setParameter(0, 0, math.random(-1000, 1000))
    setParameter(0, 1, math.random(4, 6))
    setParameter(0, 2, math.random(5, 7))
    setParameter(0, 5, 16)
    setParameter(1, 0, 1)
    addConnection(0, 0, 1, 1)
end