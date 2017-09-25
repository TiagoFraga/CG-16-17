cmake_minimum_required(VERSION 2.8)

include_directories("include")
set(ENGINE_SOURCE Engine/Engine.cpp Engine/tinyxml2.cpp)
set(GENERATOR_SOURCE Generator/Generator.cpp)

if (NOT APPLE)
	if (WIN32)
		set(TOOLKITS_FOLDER "" CACHE PATH "Path to toolkits folder")
		include_directories(
		  ${TOOLKITS_FOLDER}/glew
			${TOOLKITS_FOLDER}/glut)
		link_directories(
			${TOOLKITS_FOLDER}/glew
			${TOOLKITS_FOLDER}/glut)
	else(WIN32)
			find_package(GLEW REQUIRED)
			include_directories(${GLEW_INCLUDE_DIRS})
			link_libraries(${GLEW_LIBRARIES})
	endif(WIN32)
endif(NOT APPLE)

if (NOT WIN32)
	find_package(GLUT REQUIRED)
	include_directories(${GLUT_INCLUDE_DIR})
	link_directories(${GLUT_LIBRARY_DIRS})
	add_definitions(${GLUT_DEFINITIONS})
endif(NOT WIN32)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

add_executable(Engine ${ENGINE_SOURCE})
add_executable(Generator ${GENERATOR_SOURCE})

file(COPY sphere.3d DESTINATION ${CMAKE_BINARY_DIR}/Source)
file(COPY config.xml DESTINATION ${CMAKE_BINARY_DIR}/Source)
file(COPY teapot.patch DESTINATION ${CMAKE_BINARY_DIR}/Source)
file(COPY teapot.3d DESTINATION ${CMAKE_BINARY_DIR}/Source)

if (WIN32)
	target_link_libraries(Engine ${OPENGL_LIBRARIES} glut32 glew32)
	file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR}/Source)
	file(COPY ${TOOLKITS_FOLDER}/glew/glew32.dll DESTINATION ${CMAKE_BINARY_DIR}/Source)

else(WIN32)
	target_link_libraries(Engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
endif (WIN32)
