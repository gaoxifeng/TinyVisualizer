const std::string TexCopyFrag=
  "#version 330 core\n"
  "uniform sampler2D tex;\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "  float aTexVal=texture(tex,tc).x;\n"
  "  FragColor=vec4(aTexVal,aTexVal,aTexVal,1);\n"
  "}\n";
