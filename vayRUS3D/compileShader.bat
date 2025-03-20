glslangValidator -V shaders/baseVS.vert -o vs.spv
glslangValidator -V shaders/baseFS.frag -o fs.spv
move "vs.spv" "testContent/shaders/"
move "fs.spv" "testContent/shaders/"
pause
