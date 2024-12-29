cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)

project(libcore)

add_subdirectory(libcore)
add_subdirectory(main)
add_subdirectory(testeShader)
add_subdirectory(terrainChunks)
add_subdirectory(terrainLod)
add_subdirectory(testeRTT)
add_subdirectory(testeRTTDepth)
add_subdirectory(mirror)


