cmake_minimum_required(VERSION 3.16)
project(one LANGUAGES C CXX)

add_library(one STATIC
    src/one_arb.c
    src/one_backoff.c
    src/one_ema.c
    src/one_outlier.c
    src/one_stale.c
    src/one_vote.c
    src/one_dead.c
    src/one_hyst.c
    src/one_slew.c
    src/one_rate.c
    src/one_init.c
    src/one_window.c
    src/one_seq.c
    src/one_buf.c
    src/one_dbuf.c
    src/one_field.c
    src/one_idx.c
    src/one_quant.c
    src/one_scale.c
    src/one_set.c
    src/one_stat.c
    src/one_trace.c
)

target_include_directories(one PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(one PUBLIC c_std_99)

include(CTest)
if(BUILD_TESTING)
    add_executable(one_arb_backoff_tests
        tests/test_arb_backoff.c
    )
    target_link_libraries(one_arb_backoff_tests PRIVATE one)
    add_test(NAME one_arb_backoff_tests COMMAND one_arb_backoff_tests)
    add_executable(one_signal_tests tests/test_signal.c)
    target_link_libraries(one_signal_tests PRIVATE one)
    add_test(NAME one_signal_tests COMMAND one_signal_tests)
    add_executable(one_numeric_tests tests/test_numeric.c)
    target_link_libraries(one_numeric_tests PRIVATE one)
    add_test(NAME one_numeric_tests COMMAND one_numeric_tests)
    add_executable(one_protocol_tests tests/test_protocol.c)
    target_link_libraries(one_protocol_tests PRIVATE one)
    add_test(NAME one_protocol_tests COMMAND one_protocol_tests)
    add_executable(one_seq_tests tests/test_seq.c)
    target_link_libraries(one_seq_tests PRIVATE one)
    add_test(NAME one_seq_tests COMMAND one_seq_tests)
    add_executable(one_buf_tests tests/test_buf.c)
    target_link_libraries(one_buf_tests PRIVATE one)
    add_test(NAME one_buf_tests COMMAND one_buf_tests)
    add_executable(one_dbuf_tests tests/test_dbuf.c)
    target_link_libraries(one_dbuf_tests PRIVATE one)
    add_test(NAME one_dbuf_tests COMMAND one_dbuf_tests)
    add_executable(one_field_tests tests/test_field.c)
    target_link_libraries(one_field_tests PRIVATE one)
    add_test(NAME one_field_tests COMMAND one_field_tests)
    add_executable(one_idx_tests tests/test_idx.c)
    target_link_libraries(one_idx_tests PRIVATE one)
    add_test(NAME one_idx_tests COMMAND one_idx_tests)
    add_executable(one_quant_tests tests/test_quant.c)
    target_link_libraries(one_quant_tests PRIVATE one)
    add_test(NAME one_quant_tests COMMAND one_quant_tests)
    add_executable(one_scale_tests tests/test_scale.c)
    target_link_libraries(one_scale_tests PRIVATE one)
    add_test(NAME one_scale_tests COMMAND one_scale_tests)
    add_executable(one_set_tests tests/test_set.c)
    target_link_libraries(one_set_tests PRIVATE one)
    add_test(NAME one_set_tests COMMAND one_set_tests)
    add_executable(one_stat_tests tests/test_stat.c)
    target_link_libraries(one_stat_tests PRIVATE one)
    add_test(NAME one_stat_tests COMMAND one_stat_tests)
    add_executable(one_trace_tests tests/test_trace.c)
    target_link_libraries(one_trace_tests PRIVATE one)
    add_test(NAME one_trace_tests COMMAND one_trace_tests)
    add_executable(one_headers_tests tests/test_headers.c)
    target_link_libraries(one_headers_tests PRIVATE one)
    add_test(NAME one_headers_tests COMMAND one_headers_tests)
    add_executable(one_headers_cpp_tests tests/test_headers.cpp)
    target_link_libraries(one_headers_cpp_tests PRIVATE one)
    add_test(NAME one_headers_cpp_tests COMMAND one_headers_cpp_tests)

endif()

option(ONE_BUILD_FOOTPRINT_BENCHMARKS "Build standalone one.h static-link footprint benchmarks" OFF)

if(ONE_BUILD_FOOTPRINT_BENCHMARKS)
    function(one_add_footprint_case name case_number)
        add_executable(${name} tests/footprint/footprint_case.c)
        target_compile_definitions(${name} PRIVATE ONE_FOOTPRINT_CASE=${case_number})
        target_link_libraries(${name} PRIVATE one)
        if(MSVC)
            target_link_options(${name} PRIVATE /MAP)
        endif()
    endfunction()

    one_add_footprint_case(one_footprint_baseline 0)
    one_add_footprint_case(one_footprint_arb 1)
    one_add_footprint_case(one_footprint_backoff 2)
    one_add_footprint_case(one_footprint_ema 3)
    one_add_footprint_case(one_footprint_outlier 4)
    one_add_footprint_case(one_footprint_stale 5)
    one_add_footprint_case(one_footprint_vote 6)
    one_add_footprint_case(one_footprint_dead 7)
    one_add_footprint_case(one_footprint_hyst 8)
    one_add_footprint_case(one_footprint_slew 9)
    one_add_footprint_case(one_footprint_rate 10)
    one_add_footprint_case(one_footprint_init 11)
    one_add_footprint_case(one_footprint_window 12)
    one_add_footprint_case(one_footprint_seq 13)
    one_add_footprint_case(one_footprint_buf 14)
    one_add_footprint_case(one_footprint_dbuf 15)
    one_add_footprint_case(one_footprint_field 16)
    one_add_footprint_case(one_footprint_idx 17)
    one_add_footprint_case(one_footprint_quant 18)
    one_add_footprint_case(one_footprint_scale 19)
    one_add_footprint_case(one_footprint_set 20)
    one_add_footprint_case(one_footprint_stat 21)
    one_add_footprint_case(one_footprint_trace 22)
    one_add_footprint_case(one_footprint_combo_numeric 101)
    one_add_footprint_case(one_footprint_combo_signal 102)
    one_add_footprint_case(one_footprint_combo_protocol 103)
    one_add_footprint_case(one_footprint_combo_storage 104)
    one_add_footprint_case(one_footprint_combo_all 105)
endif()
