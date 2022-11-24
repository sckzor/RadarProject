cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK
include(pico_sdk_import.cmake)

project(adctest C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_executable(usbtest
	usbtest.c
	usb_descriptors.c
	)

target_include_directories(usbtest PUBLIC
	${CMAKE_CURRENT_LIST_DIR})

# Pull in pico_stdlib
target_link_libraries(usbtest pico_stdlib)
target_link_libraries(usbtest tinyusb_device)
target_link_libraries(usbtest tinyusb_board)

# create binaries
pico_add_extra_outputs(usbtest)
