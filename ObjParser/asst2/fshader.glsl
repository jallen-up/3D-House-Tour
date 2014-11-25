attribute vec4 color;
attribute vec2 texCoord;
varying vec4 fColor;
uniform sampler2D texture;
void main(){
fColor = color*texture(texture,texCoord);
}