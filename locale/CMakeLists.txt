# Add your language code to this list
set(LOCALES)


find_package(Gettext REQUIRED)

foreach(LOCALE ${LOCALES})
    gettext_process_po_files(${LOCALE} ALL
        PO_FILES ${LOCALE}/main.po
        INSTALL_DESTINATION "${DATADIR}/locale")
endforeach()
