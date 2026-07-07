#include <metal_stdlib>

using namespace metal;

struct FrameConstants
{
    float Time;
};

float dot2(float2 a) { return dot(a, a); }

float3 palette(float t)
{
    // Color palette: [[0.798 0.088 0.308] [0.151 -0.008 0.168] [2.260 3.138 3.138] [5.529 3.134 0.236]]

    float3 a = { 0.798f, 0.088f, 0.308f };
    float3 b = { 0.151f,-0.008f, 0.168f };
    float3 c = { 2.260f, 3.138f, 3.138f };
    float3 d = { 5.529f, 3.134f, 0.236f };

    return a + b * cos(6.28318f * (c * t + d)); // THE POWER OF TRIGONOMETRY!
}

float Heart(float2 p)
{
    p.x = abs(p.x);

    if( p.y + p.x > 1.0f )
        return sqrt(dot2(p - float2(0.25f, 0.75f))) - sqrt(2.0) / 4.0;
    return sqrt(min(dot2(p - float2(0.00f, 1.00f)),
                    dot2(p - 0.5f * max(p.x + p.y, 0.0f)))) * sign(p.x - p.y);
}

kernel void PerPixel(texture2d<float, access::write> finalImage [[ texture(0) ]], uint2 coord [[ thread_position_in_grid ]], constant FrameConstants& frameConstants [[ buffer(0) ]])
{
    float aspectRatio = float(finalImage.get_width()) / float(finalImage.get_height());
    float2 resolution = float2(finalImage.get_width(), finalImage.get_height());
    float time = frameConstants.Time;
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);

    float3 col = palette(time);

    float2 uv = (float2(coord) / resolution) * 2.0f - 1.0f;
    uv.x *= aspectRatio;
    uv.y += 0.6f;

    float d = Heart(uv);
    d = abs(d);
    d = smoothstep(0.0f, 0.1f, d);
    d = pow((0.01f / d) * exp(-d), 2.0f);

    finalColor = col * d;
    
    finalImage.write(float4(finalColor, 1.0f), coord);
}