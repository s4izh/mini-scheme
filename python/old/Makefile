.PHONY: clean generate-grammar execute
.DEFAULT_GOAL := generate-grammar

clean:
	for i in $(find . -name "__pycache__"); do rm -rf $i; done

generate-grammar:
	cd src/grammar
	antlr4 -Dlanguage=Python3 -o generated MiniScheme.g4

execute:
	python3 -m src.main example.scm
