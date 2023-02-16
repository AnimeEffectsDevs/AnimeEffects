#version 140

// A is background and B is top layer
// We unsupport dissolve, darker color, lighter color, hue, saturation, color, luminosity,
// since they are non RGB formulas.

#define BlendNormal(A,B)     (B)
#define BlendLighten(A,B)    ((A > B) ? A : B)
#define BlendDarken(A,B)     ((A > B) ? B : A)
#define BlendMultiply(A,B)   (A * B)
#define BlendAverage(A,B)    ((A + B) / 2.0)
#define BlendAdd(A,B)        (min(1.0, A + B))
#define BlendSubtract(A,B)   (max(0.0, A - B))
#define BlendDifference(A,B) (abs(A - B))
#define BlendDivide(A,B)     ((B == 0.0) ? B : min(1.0, A / B))
#define BlendScreen(A,B)     (1.0 - (1.0 - A) * (1.0 - B))
#define BlendExclusion(A,B)  (A + B - 2 * A * B)
#define BlendOverlay(A,B)    ((A < 0.5) ? (2 * A * B) : (1.0 - 2 * (1.0 - A) * (1.0 - B)))
#define BlendSoftLight(A,B)  ((A < 0.5) ? (2.0 * A * B + A * A * (1.0 - 2.0 * B)) : (sqrt(A) * (2.0 * B - 1.0) + (2.0 * A) * (1.0 - B)))
#define BlendHardLight(A,B)  (BlendOverlay(B, A))
#define BlendColorDodge(A,B) ((B == 1.0) ? B : min(1.0, A / (1.0 - B)))
#define BlendColorBurn(A,B)  ((B == 0.0) ? B : max(0.0, 1.0 - (1.0 - A) / B))
#define BlendLinearDodge(A,B)(BlendAdd(A, B))
#define BlendLinearBurn(A,B) (max(0.0, A + B - 1.0))
#define BlendLinearLight(A,B)((B < 0.5) ? BlendLinearBurn(A, 2.0 * B) : BlendLinearDodge(A, 2.0 * (B - 0.5)))
#define BlendVividLight(A,B) ((B < 0.5) ? BlendColorBurn(A, 2.0 * B) : BlendColorDodge(A, 2.0 * (B - 0.5)))
#define BlendPinLight(A,B)   ((B < 0.5) ? BlendDarken(A, 2.0 * B) : BlendLighten(A, 2.0 * (B - 0.5)))
#define BlendHardMix(A,B)    ((BlendVividLight(A,B)) < 0.5 ? 0.0 : 1.0)

#variation BLEND_FUNC BlendNormal
#variation IS_CLIPPEE 0

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform sampler2D uDestTexture;

#if IS_CLIPPEE
uniform int uClippingId;
uniform usampler2D uClippingTexture;
#endif

in vec2 vTexCoord;
in vec2 vDestCoord;

out vec4 oFragColor;

vec4 blendColor(const vec4 src, const vec4 dst)
{
#if 1
    vec4 result;
    result.a = src.a;
    float idstA = 1.0 - dst.a;

    result.r = dst.a * BLEND_FUNC(dst.r, src.r) + idstA * src.r;
    result.g = dst.a * BLEND_FUNC(dst.g, src.g) + idstA * src.g;
    result.b = dst.a * BLEND_FUNC(dst.b, src.b) + idstA * src.b;
    return result;

#elif 1
    vec4 result;
    float a0 = src.a * dst.a;
    float a1 = src.a * (1.0 - dst.a);
    float a2 = dst.a * (1.0 - src.a);

    result.a = a0 + a1 + a2;
    float div = max(result.a, 1.0 / 255.0);

    result.r = (a0 * BLEND_FUNC(dst.r, src.r) + a1 * src.r + a2 * dst.r) / div;
    result.g = (a0 * BLEND_FUNC(dst.g, src.g) + a1 * src.g + a2 * dst.g) / div;
    result.b = (a0 * BLEND_FUNC(dst.b, src.b) + a1 * src.b + a2 * dst.b) / div;
    return result;

#elif 1
    float sum = src.a + dst.a;
    return vec4((src.rgb * src.a + dst.rgb * (1.0 - src.a)), sum);

#else
    return vec4(src.rgb * src.a + dst.rgb * (1.0 - src.a), 1.0);

#endif
}

void main(void)
{
    vec4 color = uColor * texture(uTexture, vTexCoord);
    ivec2 destCoord = ivec2(vDestCoord);
    vec4 destColor = texelFetch(uDestTexture, destCoord, 0);

#if IS_CLIPPEE
    uvec2 clippingData = texelFetch(uClippingTexture, destCoord, 0).xy;
    if (uClippingId == int(clippingData.x))
    {
        color.a *= float(clippingData.y) / 255.0;
        oFragColor = blendColor(color, destColor);
    }
    else
    {
        //oFragColor = destColor;
        discard;
    }
#else
    oFragColor = blendColor(color, destColor);
#endif
}
