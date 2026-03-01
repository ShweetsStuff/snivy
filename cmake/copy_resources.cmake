if(NOT DEFINED SRC_DIR OR NOT DEFINED DST_DIR)
    message(FATAL_ERROR "SRC_DIR and DST_DIR must be defined")
endif()

set(CHARACTERS_DIR "${SRC_DIR}/characters")
set(CHARACTERS_ZIP_SCRIPT "${CHARACTERS_DIR}/zip")

if(EXISTS "${CHARACTERS_ZIP_SCRIPT}")
    execute_process(
            COMMAND "${CHARACTERS_ZIP_SCRIPT}"
            WORKING_DIRECTORY "${CHARACTERS_DIR}"
            RESULT_VARIABLE ZIP_SCRIPT_RESULT
    )
    if(NOT ZIP_SCRIPT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed running ${CHARACTERS_ZIP_SCRIPT} (exit code ${ZIP_SCRIPT_RESULT})")
    endif()
endif()

file(REMOVE_RECURSE "${DST_DIR}")
file(MAKE_DIRECTORY "${DST_DIR}")

# Copy all resources except characters/ contents.
file(COPY "${SRC_DIR}/" DESTINATION "${DST_DIR}"
        PATTERN "characters/*" EXCLUDE)

# Copy only .zip archives from resources/characters.
file(MAKE_DIRECTORY "${DST_DIR}/characters")
file(GLOB CHARACTER_ZIPS "${CHARACTERS_DIR}/*.zip")
if(CHARACTER_ZIPS)
    file(COPY ${CHARACTER_ZIPS} DESTINATION "${DST_DIR}/characters")
endif()
