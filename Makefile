BINARIES = forftanks designer.cgi
HTML = forf.html procs.html intro.html designer.html chord.html
WWW = style.css grunge.png designer.js figures.js tanks.js nav.html.inc
SCRIPTS = killmatrix.pl rank.awk summary.awk winner.awk

CFLAGS = -Wall -std=gnu90 -g

all: $(BINARIES) $(HTML)

forftanks: forftanks.o ctanks.o forf.o tankdir.o tankjson.o
forftanks: LDLIBS = -lm -ljansson

forftanks.o: forf.h ctanks.h tankdef.h tankdir.h tankjson.h
forf.o: forf.c forf.h
ctanks.o: ctanks.h
tankdir.o: tankdef.h tankdir.h
tankjson.o: tankdef.h tankjson.h

%.html: %.html.m4
	m4 $< > $@

.PHONY: install clean check-env
install: check-env
	install -d $(DESTDIR)/bin
	install run-tanks $(DESTDIR)/bin
	install forftanks $(DESTDIR)/bin
	install $(SCRIPTS) $(DESTDIR)/bin
	install -d $(DOCROOT)
	install designer.cgi $(CGIBIN)
	install $(HTML) $(DOCROOT)
	install $(WWW) $(DOCROOT)
	cp -r examples $(DOCROOT)/examples

clean:
	rm -f *.o next-round round-*.html round-*.json results-*.txt current.html
	rm -f $(BINARIES) $(HTML)

check-env:
ifndef DESTDIR
	$(error DESTDIR is undefined)
endif
ifndef DOCROOT
ifndef DESTDIR
	$(error DOCROOT is undefined)
else
DOCROOT = $(DESTDIR)
endif
endif
ifndef CGIBIN
ifndef DESTDIR
	$(error CGIBIN is undefined)
else
CGIBIN = $(DESTDIR)
endif
endif
