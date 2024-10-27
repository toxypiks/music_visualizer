#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    float x = fragTexCoord.x;
    float y = fragTexCoord.y;
    finalColor = vec4(x, y, 0 ,1);
}