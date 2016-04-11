#!/bin/zsh

xelatex main
bibtex main
xelatex main
xelatex main
rm main.aux main.bbl main.blg main.ent main.loa main.lof main.log main.lot main.out main.toc
