#version 300 es
precision mediump float;
uniform sampler2D s_tex0;
in vec2 v_TexCoord;
out vec4 fragColor;
void main()
{
//    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    fragColor = texture2D(s_tex0, v_TexCoord) * vec4(1.0, 1.0, 1.0, 1.0);
}
