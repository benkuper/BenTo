#version 150



out vec4 outputColor;

//perso
uniform int inverse;
uniform vec2 fboSize;
uniform vec2 sourceSize;
uniform sampler2DRect sourceTex;
uniform sampler2DRect ledMap;
//uniform float pixMaps[65536];

void main()
{
    // gl_FragCoord contains the window relative coordinate for the fragment.
    // we use gl_FragCoord.x position to control the red color value.
    // we use gl_FragCoord.y position to control the green color value.
    // please note that all r, g, b, a values are between 0 and 1.
    
	//ivec2 lsize = textureSize(ledMap,0);
    //ivec2 tsize = textureSize(sourceTex,0);
    
    vec2 lsize = fboSize; //led map is same size as fbo
    vec2 tsize = sourceSize;
    
	float tx = (gl_FragCoord.x / fboSize.x) * lsize.x;
	float ty = (gl_FragCoord.y / fboSize.y) * lsize.y;

	vec4 tcoord = texture(ledMap,vec2(tx,ty));

    if(inverse == 1) tcoord.y = 1-tcoord.y;
	outputColor = texture(sourceTex,vec2(tcoord.x*tsize.x,tcoord.y*tsize.y));
    
}

