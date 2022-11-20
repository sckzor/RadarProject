cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK
include(pico_sdk_import.cmake)

project(adctest C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_executable(adctest
	adctest.c
	)

# Pull in pico_stdlib
target_link_libraries(adctest pico_stdlib)
target_link_libraries(adctest hardware_adc)

pico_enable_stdio_usb(adctest 1)  
pico_enable_stdio_uart(adctest 0)


# create binaries
pico_add_extra_outputs(adctest)
