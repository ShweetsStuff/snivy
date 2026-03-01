if(NOT DEFINED SRC_DIR OR NOT DEFINED DST_DIR)
    message(FATAL_ERROR "SRC_DIR and DST_DIR must be defined")
endif()

file(REMOVE_RECURSE "${DST_DIR}")
file(MAKE_DIRECTORY "${DST_DIR}")

# Copy all resources except characters/ contents.
file(COPY "${SRC_DIR}/" DESTINATION "${DST_DIR}"
        PATTERN "characters/*" EXCLUDE)

# Copy only .zip archives from resources/characters.
file(MAKE_DIRECTORY "${DST_DIR}/characters")
file(GLOB CHARACTER_ZIPS "${SRC_DIR}/characters/*.zip")
if(CHARACTER_ZIPS)
    file(COPY ${CHARACTER_ZIPS} DESTINATION "${DST_DIR}/characters")
endif()
