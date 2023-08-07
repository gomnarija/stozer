#source files
set(SOURCE_FILES 
${SOURCE_DIR}/stozer.cpp
${SOURCE_DIR}/utils.cpp
${SOURCE_DIR}/process.cpp
${SOURCE_DIR}/userspace/argumentator.cpp
${SOURCE_DIR}/userspace/configurator.cpp
${SOURCE_DIR}/userspace/krsh.cpp
${SOURCE_DIR}/userspace/kalendar.cpp
${SOURCE_DIR}/userspace/fs/gde.cpp
${SOURCE_DIR}/userspace/fs/idi.cpp
${SOURCE_DIR}/userspace/fs/listaj.cpp
${SOURCE_DIR}/userspace/fs/napravi.cpp
${SOURCE_DIR}/userspace/fs/ukloni.cpp
${SOURCE_DIR}/userspace/fs/pomeri.cpp
${SOURCE_DIR}/userspace/fs/kopiraj.cpp
${SOURCE_DIR}/userspace/uredi/uredi.cpp
${SOURCE_DIR}/userspace/uredi/tui.cpp
${SOURCE_DIR}/userspace/uredi/input.cpp
${SOURCE_DIR}/userspace/uredi/files.cpp
${SOURCE_DIR}/userspace/sat.cpp)
set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
#header files
set(HEADER_FILES
${SOURCE_DIR}/stozer.h
${SOURCE_DIR}/utils.h
${SOURCE_DIR}/userspace/argumentator.h
${SOURCE_DIR}/userspace/configurator.h
${SOURCE_DIR}/userspace/krsh.h
${SOURCE_DIR}/userspace/kalendar.h
${SOURCE_DIR}/userspace/fs/gde.h
${SOURCE_DIR}/userspace/fs/idi.h
${SOURCE_DIR}/userspace/fs/listaj.h
${SOURCE_DIR}/userspace/fs/napravi.h
${SOURCE_DIR}/userspace/fs/ukloni.h
${SOURCE_DIR}/userspace/fs/pomeri.h
${SOURCE_DIR}/userspace/fs/kopiraj.h
${SOURCE_DIR}/userspace/uredi/uredi.h
${SOURCE_DIR}/userspace/sat.h)
set(HEADER_FILES ${HEADER_FILES} PARENT_SCOPE)
