set(HEADERS
    ${CORE_DIR}/brep/BREPTypes.h
    ${CORE_DIR}/brep/Edge.h
    ${CORE_DIR}/brep/Face.h
    ${CORE_DIR}/brep/Vertex.h
    ${CORE_DIR}/brep/HalfEdge.h
    ${CORE_DIR}/brep/Loop.h
    ${CORE_DIR}/brep/Shell.h
    ${CORE_DIR}/brep/Solid.h
    ${CORE_DIR}/brep/BREPBuilder.h
)
set(SOURCES
    ${CORE_DIR}/brep/Edge.cpp
    ${CORE_DIR}/brep/Face.cpp
    ${CORE_DIR}/brep/Vertex.cpp
    ${CORE_DIR}/brep/HalfEdge.cpp
    ${CORE_DIR}/brep/Loop.cpp
    ${CORE_DIR}/brep/Shell.cpp
    ${CORE_DIR}/brep/Solid.cpp
    ${CORE_DIR}/brep/BREPBuilder.cpp
)

add_library(MizanCore STATIC
    ${HEADERS}
    ${SOURCES}
)

target_include_directories(MizanCore PUBLIC
    ${CORE_DIR}
)