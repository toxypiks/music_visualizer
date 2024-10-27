#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    float r = 0.5;
    float x = fragTexCoord.x - 0.5;
    float y = fragTexCoord.y - 0.5;
    if(x*x + y*y <= r*r) {
	  finalColor = fragColor;
    } else {
        finalColor = vec4(0);
    }
}
