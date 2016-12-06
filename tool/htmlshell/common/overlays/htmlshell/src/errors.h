#pragma once

// Error codes returned by browser process main function

#define HTMLSHELL_NO_ERROR                    0
#define HTMLSHELL_NO_MANIFEST_OR_URL          1
#define HTMLSHELL_INVALID_MANIFEST            2
#define HTMLSHELL_NOT_INITIALIZED             3
#define HTMLSHELL_INITIALIZED_TWICE           4
#define HTMLSHELL_INITIALIZATION_ERROR        5
#define HTMLSHELL_FAIL_TO_LOAD_API            6
#define HTMLSHELL_API_NOT_FOUND               7
#define HTMLSHELL_LIBRARY_LOAD_ERROR          8
#define HTMLSHELL_LIBRARY_INIT_ERROR          9
#define HTMLSHELL_BROWSER_PROCESS_INIT_ERROR 10
#define HTMLSHELL_LIBRARY_RUNTIME_ERROR      11
#define HTMLSHELL_JAVASCRIPT_SYNTAX_ERROR    12
#define HTMLSHELL_EXTQUERY_FORMAT_ERROR      13
#define HTMLSHELL_IPC_ERROR                  14

// New error codes must be added before HTMLSHELL_EXTERNAL_CODE_BASE.
// Do NOT add error codes after HTMLSHELL_EXTERNAL_CODE_NO_ERROR as it is used
// as the base code for custom extensions errors.
#define HTMLSHELL_EXTERNAL_CODE_BASE         15 // Base error code for shell extensions (libraries)
#define HTMLSHELL_EXTERNAL_CODE_NO_ERROR     (HTMLSHELL_EXTERNAL_CODE_BASE)
