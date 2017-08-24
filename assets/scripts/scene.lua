local function rgb(r,g,b)
        return {r=r/255.0, g=g/255.0, b=b/255.0}
end

-- ambient = rgb(12, 59, 110)
-- diffuse = rgb(218, 226, 161)
-- specular = rgb(70, 79, 90)

-- ambient = rgb(21, 38, 79)
-- diffuse = rgb(162, 250, 233)
-- specular = rgb(252, 255, 213)

ambient = rgb(57, 97, 108)
diffuse = rgb(252, 233, 232)
specular = rgb(0, 0, 0)

setSceneAmbient(ambient.r, ambient.g, ambient.b)
setSceneDiffuse(diffuse.r, diffuse.g, diffuse.b)
setSceneSpecular(specular.r, specular.g, specular.b)

print(boop.boop());
