const std::string ShadowFrag=
  "#version 330 core\n"
  "uniform vec3 lightPos;\n"
  "uniform float far_plane;\n"
  "in vec3 FragPos;\n"
  "void main() {\n"
  "  float lightDistance=length(FragPos-lightPos);\n"
  "  //map to [0;1] range by dividing by far_plane\n"
  "  lightDistance=lightDistance/far_plane;\n"
  "  //write this as modified depth\n"
  "  gl_FragDepth=lightDistance;\n"
  "}\n";
