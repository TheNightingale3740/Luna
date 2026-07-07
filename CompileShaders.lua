local ShaderSourceDir = "Radiance/Source/Radiance/Renderer/Shaders/"
local ShaderIntermediateDir = "Radiance/Source/Radiance/Res/Shaders/bin-int/"
local ShaderOutputDir = "Radiance/Source/Radiance/Res/Shaders/"

local SourceFiles = os.matchfiles(ShaderSourceDir .. "**.metal")

if #SourceFiles == 0 then
    print("No shader source files to compile.")
    os.exit(0)
end

os.mkdir(ShaderOutputDir)
os.mkdir(ShaderIntermediateDir)

for i, v in ipairs(SourceFiles) do
    -- Compile Shader
    local outFile = ShaderIntermediateDir .. path.getbasename(v) .. ".air"
    local compileCommand = "xcrun -sdk macosx metal -c " .. v .. " -o " .. outFile

    -- Handle errors
    local success, _, code = os.execute(compileCommand)
    if not success then
        os.exit(code)
    end
end

local IntermediateFiles = os.matchfiles(ShaderIntermediateDir .. "**.air")

-- Link shader program
local success, _, code = os.execute("xcrun -sdk macosx metallib " .. table.concat(IntermediateFiles, " ") .. " -o " .. ShaderOutputDir .. "PathTracingShader.metallib")
if not success then
    os.exit(code)
end

-- Clean up intermediate files
for i, v in ipairs(IntermediateFiles) do
    os.remove(v)
end

os.rmdir(ShaderIntermediateDir)