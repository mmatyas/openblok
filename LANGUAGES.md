LANGUAGES
=========

You can translate openblok by creating translation files in GNU gettext format.

```bash
  # create/update base files
  
  cd locale/
  # find relevant files (in this case using ripgrep) and save them into locale/POTFILES.in
  rg -l 'tr\("\w+' ../src > POTFILES.in 
  
  # scan all relevant files for usages of tr() and extract strings
  xgettext --keyword=tr --language=C --add-comments --sort-output -o main.pot --files-from=POTFILES.in
  
  # example for german (de) locale
  # option1: if new language
  msginit --input=main.pot --locale=de --output=de/main.po
  
  # option 2: updating existing translations
  msgmerge --update de/main.po main.pot

  # create translation file for locale
  msgfmt --output-file=de/LC_MESSAGES/main.mo de/main.po
```