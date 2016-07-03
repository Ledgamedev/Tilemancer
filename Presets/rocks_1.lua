function init()
    setName("Rocks 1")
    setDesc("Sets up rocks")
end

function apply()
    math.randomseed(os.time())
    addNode("cells.lua", -70, -95)
    addNode("perlin_noise.lua", -70, 112)
    addNode("blend.lua", 50, 0)
    setParameter(0, 0, math.random(-1000, 1000))
    setParameter(0, 2, 100)
    setParameter(0, 3, 200)
    setParameter(1, 0, math.random(-1000, 1000))
    setParameter(1, 1, 4)
    setParameter(1, 4, 70)
    setParameter(1, 5, 45)
    setParameter(2, 0, 5)
    addConnection(0, 0, 2, 0)
    addConnection(1, 0, 2, 1)
end