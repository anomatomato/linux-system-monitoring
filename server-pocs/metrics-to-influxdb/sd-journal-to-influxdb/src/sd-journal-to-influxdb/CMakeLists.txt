add_library(sd-journal-to-influxdb sd_journal_to_influxdb.c sd_journal_to_influxdb.h)
target_include_directories(sd-journal-to-influxdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sd-journal-to-influxdb publish-influxdb)
