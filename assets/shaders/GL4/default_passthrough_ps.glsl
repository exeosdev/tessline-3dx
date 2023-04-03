
#version 430 core

smooth in vec4 psColor;
smooth in vec2 psTexCoord0;

layout( location = 0 ) out vec4 outPixelColor;

void main()
{
	outPixelColor = vec4( 1.0f ); // psColor;
}
