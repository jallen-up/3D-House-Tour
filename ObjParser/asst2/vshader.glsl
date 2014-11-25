attribute  vec4 vPosition;
attribute  vec3 vNormal;
attribute vec2 vTexCoord;
varying vec2 texCoord;
varying vec4 color;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform float Shininess;
void main(){
texCoord = vTexCoord;
vec3 pos = (ModelView * vPosition).xyz;
vec3 L = normalize(LightPosition.xyz - pos);
vec3 E = normalize(-pos);
vec3 H = normalize(L + E);
vec3 N = vNormal;
vec4 ambient = AmbientProduct;
float Kd = max(dot(L, N), 0.0);
vec4  diffuse = Kd*DiffuseProduct;
float Ks = pow(max(dot(N, H), 0.0), Shininess);
vec4  specular = Ks * SpecularProduct;
if (dot(L, N) < 0.0) {
specular = vec4(0.0, 0.0, 0.0, 1.0);
}
gl_Position = Projection * ModelView * vPosition;
color = ambient + diffuse + specular;
color.a = 1.0;
}