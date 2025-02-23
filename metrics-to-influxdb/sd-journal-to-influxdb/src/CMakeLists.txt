add_subdirectory(sd-journal-to-influxdb)
add_subdirectory(mqtt)
add_subdirectory(publish)

add_executable(journal-to-influxdb main.c)
target_link_libraries(${PROJECT_NAME} sd-journal-to-influxdb)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
