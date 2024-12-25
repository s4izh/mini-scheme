.PHONY: clean generate-grammar
.DEFAULT_GOAL := generate-grammar

clean:
	@find . -name "__pycache__" -type d -exec rm -rf {} +
	@rm -rf generated

generate-grammar:
	antlr4 -Dlanguage=Python3 -o generated scheme.g4


test:
	python3 scheme.py < tests/suma.scm
	python3 scheme.py < tests/suma-function.scm
