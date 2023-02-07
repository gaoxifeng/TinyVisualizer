const std::string XORFrag=
  "#version 330 core\n"
  "uniform sampler2D tex[2];\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "  float aTexVal=texture(tex[0],tc).x;\n"
  "  float bTexVal=texture(tex[1],tc).x;\n"
  "  float xor=max(0,aTexVal-bTexVal)+max(0,bTexVal-aTexVal);\n"
  "  FragColor=vec4(xor,xor,xor,1);\n"
  "}\n";
