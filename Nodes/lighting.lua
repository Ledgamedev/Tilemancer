function init()
    setName("Lighting")
    setDesc("Applies lighting to a heightmap")
    setSize(100, 24+64+8+8+18+18+7+4+4+64+68)
    addOutput(24+32)
    addOutput(24+32+4+64)
    addOutput(24+32+4+64+68)
    addInput("Heightmap", 24+64+8+8+4+64+68)
    addParameter("Direction", "Light direction", 24+64+8+8+18+4+64+68, -22, -1, -1)
    addParameter("AO", "Ambient occlusion intensity", 24+64+8+8+18+18+4+64+68, 10, 0, 100, true)
end

function normalize(x, y, z)
    l = math.sqrt(x*x+y*y+z*z)
    return x/l, y/l, z/l
end

function cross(ax, ay, az, bx, by, bz)
    return ay*bz-az*by, az*bx-ax*bz, ax*by-ay*bx
end

function dot(ax, ay, az, bx, by, bz)
    return ax*bx+ay*by+az*bz
end

function apply()
    tileSize = getTileSize()

    intensity = getValue(2, 0, 0, 100.0)
    radius = math.floor(tileSize*intensity)
    kernel = { }
    a = 1.0/(2.0*radius*radius)
    b = 1.0/(math.sqrt(2.0*math.pi)*radius)
    c = 1.0
    r = -radius
    total = 0
    for i=0, radius*2 do
        x = r*r
        kernel[i] = b*math.exp(-x*b)
        total = total+kernel[i]
        r = r+1
    end
    for i=0, radius*2 do
        kernel[i] = kernel[i]/total
        if radius==0 then
            kernel[i] = 1
        end
    end
    blur = { }
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        sum = 0;
        for bx=0, radius*2 do
            c = getValue(0, x-radius+bx, y, 1.0)
            sum = sum+c*kernel[bx]
        end
        blur[i] = sum
    end
    blur2 = { }
    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)
        sum = 0;
        for by=0, radius*2 do
            ny = y-radius+by
            while ny<0 do ny=ny+tileSize end
            while ny>=tileSize do ny=ny-tileSize end
            index = ny*tileSize+x
            c = blur[index]
            sum = sum+c*kernel[by]
        end
        blur2[i] = sum
    end

    for i=0, tileSize*tileSize-1 do
        x = i%tileSize
        y = math.floor(i/tileSize)

        s11 = getValue(0, x, y, 1)
        s01 = getValue(0, x-1, y, 1)
        s21 = getValue(0, x+1, y, 1)
        s10 = getValue(0, x, y-1, 1)
        s12 = getValue(0, x, y+1, 1)
        vax, vay, vaz = normalize(128/tileSize, 0, s21-s01)
        vbx, vby, vbz = normalize(0, 128/tileSize, s12-s10)
        nx, ny, nz = normalize(cross(vax, vay, vaz, vbx, vby, vbz))
        
        direction = getValue(1, 0, 0, 1)*math.pi/180.0
        lx, ly, lz = normalize(math.sin(direction), -math.cos(direction), 0.2)
        d = math.max(0.0, math.min(1.0, dot(lx, ly, lz, nx, ny, nz)));
        
        ao = s11+1.0-blur2[i]
        
        vax2, vay2, vaz2 = normalize(32/tileSize, 0, s21-s01)
        vbx2, vby2, vbz2 = normalize(0, 32/tileSize, s12-s10)
        nx2, ny2, nz2 = cross(vax2, vay2, vaz2, vbx2, vby2, vbz2)

        setPixel(0, x, y, d*ao, d*ao, d*ao)
        setPixel(1, x, y, (nx2+1)/2, (-ny2+1)/2, (nz2+1)/2)
        setPixel(2, x, y, ao, ao, ao)
    end
end