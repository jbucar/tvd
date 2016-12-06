message("Start generation of htmlshellremote node module")

set(NODE_DIR ${BIN_DIR}/htmlshellremote)
set(NODE_TEST_DIR ${BIN_DIR}/htmlshellremote/test)

# Create directory structure
FILE(MAKE_DIRECTORY ${NODE_TEST_DIR}/node_modules)

# Copy src files
FILE(COPY ${SRC_DIR}/node/package.json
          ${SRC_DIR}/htmlshell.proto
          ${SRC_DIR}/node/src/htmlshellremote.js
     DESTINATION ${NODE_DIR})

# Copy test files
FILE(COPY ${SRC_DIR}/node/test/
     DESTINATION ${NODE_TEST_DIR}
     FILES_MATCHING PATTERN "*.js")

# Copy libhtmlshellmock.so to htmlshellremote/test/node_modules and rename to htmlshellmock.node
FILE(COPY ${BIN_DIR}/libhtmlshellmock.so
     DESTINATION ${NODE_TEST_DIR}/node_modules)
FILE(RENAME ${NODE_TEST_DIR}/node_modules/libhtmlshellmock.so
            ${NODE_TEST_DIR}/node_modules/htmlshellmock.node)

# Copy protobufjs node module
FILE(COPY ${SRC_DIR}/node/node_modules
     DESTINATION ${NODE_DIR})

message("htmlshellremote node module generated successfully")
