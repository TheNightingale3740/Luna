local PlistContents = [[


<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>Radiance</string>

    <key>CFBundleDisplayName</key>
    <string>RadianceEditor</string>

    <key>CFBundleIdentifier</key>
    <string>com.yourname.radiance</string>

    <key>CFBundleVersion</key>
    <string>1.0.0</string>

    <key>CFBundleShortVersionString</key>
    <string>1.0</string>

    <key>CFBundlePackageType</key>
    <string>APPL</string>

    <key>CFBundleExecutable</key>
    <string>RadianceEditor</string>

    <key>NSHighResolutionCapable</key>
    <true/>

    <key>CFBundleIconFile</key>
    <string>Icon</string>

    <key>LSMinimumSystemVersion</key>
    <string>26.0</string>
</dict>
</plist>


]]

local DestinationFolder = arg[1]

os.execute("mkdir -p ../" .. DestinationFolder)

local file = io.open("../" .. DestinationFolder .. "/Info.plist", "w")
if file then
    file:write(PlistContents)
end

file:close()

local pkgInfo = io.open("../" .. DestinationFolder .. "/PkgInfo", "w")
if pkgInfo then
   pkgInfo:write("APPL????") 
end

pkgInfo:close()