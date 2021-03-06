
include_directories("include")

set(GENERATOR_SOURCE Generator/Generator.cpp)
set(ENGINE_SOURCE Engine/Engine.cpp Engine/tinyxml2.cpp)

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})

if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

add_executable(Engine ${ENGINE_SOURCE})
add_executable(Generator ${GENERATOR_SOURCE})

target_link_libraries(Engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
