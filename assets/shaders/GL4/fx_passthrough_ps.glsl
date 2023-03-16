
#version 430 core

in vec2 psTexCoord0;

layout( binding = 1 ) uniform sampler2D uSampler0;

layout( location = 0 ) out vec4 outPixelColor;

void main()
{
	vec4 sampledColor = texture( uSampler0, psTexCoord0 );
	outPixelColor = vec4( sampledColor.xyz, 1.0f );
}

