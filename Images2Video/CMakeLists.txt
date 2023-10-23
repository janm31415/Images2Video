if (WIN32)
  set(FFMPEG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/include)
  list(APPEND FFMPEG_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/avcodec.lib
    #${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/avdevice.lib 
    #${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/avfilter.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/avformat.lib 
    ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/avutil.lib 
    #${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/postproc.lib 
    #${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/swresample.lib 
    ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/lib/swscale.lib 
  )
endif (WIN32)

if (UNIX)
  if (APPLE)
    link_directories(/opt/homebrew/Cellar/ffmpeg/6.0_1/lib)
    set(FFMPEG_INCLUDE_DIR /opt/homebrew/Cellar/ffmpeg/6.0_1/include)
    list(APPEND FFMPEG_LIBRARIES
    avcodec avformat avutil swscale)
  else(APPLE)
  endif(APPLE)
endif (UNIX)

set(STB
${CMAKE_CURRENT_SOURCE_DIR}/../stb/stb_image.h
${CMAKE_CURRENT_SOURCE_DIR}/../stb/stb_image_write.h
)


set(HDRS
image.h
video_writer.h
    )
	
set(SRCS
image.cpp
main.cpp
video_writer.cpp
)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi")
endif(WIN32)

include ("../jtk/jtk/jtk.cmake")

# general build definitions
add_definitions(-DNOMINMAX)
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(Images2Video ${HDRS} ${SRCS} ${STB})

source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})
source_group("ThirdParty/stb" FILES ${STB})

 target_include_directories(Images2Video
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../jtk
    ${FFMPEG_INCLUDE_DIR}
    )	
	
target_link_libraries(Images2Video
    PRIVATE	
    ${FFMPEG_LIBRARIES}
    )	

if (WIN32)
add_custom_command(TARGET Images2Video POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/bin" "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(TARGET Images2Video POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg-win64-shared/bin" "$<TARGET_FILE_DIR:Images2Video>")
endif (WIN32)
