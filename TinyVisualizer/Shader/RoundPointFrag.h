const std::string RoundPointFrag=
  "#version 330 core\n"
  "uniform vec4 diffuse;\n"
  "uniform bool useTexture;\n"
  "uniform sampler2D diffuseMap;\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main(void)\n"
  "{\n"
  "  vec2 circCoord=2.0*gl_PointCoord-1.0;\n"
  "  if(dot(circCoord,circCoord)>1.0)\n"
  "     discard;\n"
  "  if(useTexture)\n"
  "    FragColor=diffuse*texture(diffuseMap,tc);\n"
  "  else FragColor=diffuse;\n"
  "}\n";
