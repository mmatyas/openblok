LANGUAGES
=========

You can translate openblok by creating translation files in GNU gettext format.

```bash
  # create/update base files
  
  cd src/
  # find relevant files (in this case using ripgrep) and save them into po/POTFILES.in
  rg -l 'tr\("\w+' . > po/POTFILES.in 
  
  # scan all relevant files for usages of tr() and extract strings
  xgettext --keyword=tr --language=C --add-comments --sort-output -o po/main.pot --files-from=po/POTFILES.in
  
  # example for german (de) locale
  # option1: if new language
  msginit --input=po/main.pot --locale=de --output=po/de/main.po
  
  # option 2: updating existing translations
  msgmerge --update po/de/main.po po/main.pot

  # create translation file for locale
  msgfmt --output-file=po/de/LC_MESSAGES/main.mo po/de/main.po
```