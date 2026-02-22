cmake_minimum_required(VERSION 3.20)
project(NeuronEngine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    NeuronArrayBase.cpp
    NeuronBase.cpp
    SynapseBase.cpp
    pch.cpp
)

# dllmain.cpp is Windows-only (its content is already guarded, but skip on
# non-MSVC to avoid the empty translation-unit warning)
if(WIN32)
    list(APPEND SOURCES dllmain.cpp)
endif()

add_library(NeuronEngine SHARED ${SOURCES})

target_include_directories(NeuronEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ── Platform-specific configuration ──────────────────────────────────────────
if(WIN32)
    # MSVC ships ppl.h / concurrent_queue.h — no external dependency needed.
    # Use the MSVC precompiled-header mechanism.
    if(MSVC)
        target_precompile_headers(NeuronEngine PRIVATE pch.h)
    endif()
else()
    # Linux / macOS: use oneTBB for parallel_for and concurrent_queue.
    find_package(TBB REQUIRED)
    target_link_libraries(NeuronEngine PRIVATE TBB::tbb)

    # Hide all symbols by default; only NEURO_EXPORT ones are visible.
    set_target_properties(NeuronEngine PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
    )

    # pch.cpp just includes pch.h → framework.h which is now guarded, so it
    # compiles to an empty TU — suppress the "no object file" warning.
    set_source_files_properties(pch.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
