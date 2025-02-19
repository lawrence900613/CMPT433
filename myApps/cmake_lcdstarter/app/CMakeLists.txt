# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(hello_world ${MY_SOURCES})
find_library(GPIOD_LIBRARY gpiod) 
# Make use of the libraries
target_link_libraries(hello_world LINK_PRIVATE hal)
target_link_libraries(hello_world LINK_PRIVATE lcd)
target_link_libraries(hello_world LINK_PRIVATE lgpio)
target_link_libraries(hello_world LINK_PRIVATE gpiod) # May need to change to HAL lib.
# Enable PThread library for linking
add_compile_options(-pthread)
add_link_options(-pthread)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET hello_world POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:hello_world>"
     "~/cmpt433/public/myApps/hello_world" 
  COMMENT "Copying ARM executable to public NFS directory")
