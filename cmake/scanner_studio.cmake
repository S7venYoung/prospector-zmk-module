# Scanner Studio is USB management, independent of keyboard HID endpoints.
# Generate scanner-only source copies; do not modify the west ZMK checkout.
set(studio_dir "${APPLICATION_SOURCE_DIR}/src/studio")
set(scanner_studio_dir "${PROJECT_BINARY_DIR}/scanner_studio")
file(MAKE_DIRECTORY "${scanner_studio_dir}")
file(READ "${studio_dir}/rpc.c" scanner_rpc)
set(endpoint_line "enum zmk_transport transport = zmk_endpoint_get_selected().transport;")
string(FIND "${scanner_rpc}" "${endpoint_line}" endpoint_pos)
if(endpoint_pos EQUAL -1)
  message(FATAL_ERROR "Studio transport selection changed upstream; audit scanner USB adaptation")
endif()
string(REPLACE "${endpoint_line}" "enum zmk_transport transport = ZMK_TRANSPORT_USB;"
       scanner_rpc "${scanner_rpc}")
file(WRITE "${scanner_studio_dir}/rpc.c" "${scanner_rpc}")

file(READ "${studio_dir}/uart_rpc_transport.c" scanner_uart)
set(tx_finish "ring_buf_get_finish(tx_buf, MAX(sent, 0));")
string(FIND "${scanner_uart}" "${tx_finish}" tx_pos)
if(tx_pos EQUAL -1)
  message(FATAL_ERROR "Studio UART TX changed upstream; audit scanner backpressure adaptation")
endif()
string(REPLACE "${tx_finish}"
  "ring_buf_get_finish(tx_buf, MAX(sent, 0));\n            if (sent <= 0) { break; }"
  scanner_uart "${scanner_uart}")
file(WRITE "${scanner_studio_dir}/uart_rpc_transport.c" "${scanner_uart}")
set_source_files_properties(
  "${studio_dir}/rpc.c" "${studio_dir}/uart_rpc_transport.c"
  TARGET_DIRECTORY app PROPERTIES HEADER_FILE_ONLY ON)
target_sources(app PRIVATE "${scanner_studio_dir}/rpc.c"
                           "${scanner_studio_dir}/uart_rpc_transport.c")
target_include_directories(app PRIVATE "${studio_dir}")
