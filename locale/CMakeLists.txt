set(LOCALES)

foreach(LOCALE ${LOCALES})
    gettext_process_po_files(${LOCALE} ALL
        PO_FILES ${LOCALE}/main.po
        INSTALL_DESTINATION "${DATADIR}/locale")
endforeach()
