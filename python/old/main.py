import sys
from os.path import dirname, abspath

# Add the src directory to sys.path
# sys.path.insert(0, abspath(dirname(__file__) + "/src"))

from antlr4 import FileStream, CommonTokenStream
from src.grammar.generated.MiniSchemeLexer import MiniSchemeLexer
from src.grammar.generated.MiniSchemeParser import MiniSchemeParser
from src.interpreter.evaluator import Evaluator

def read_and_print_file(filename):
    try:
        with open(filename, 'r') as file:
            content = file.read()
            print(content)
    except FileNotFoundError:
        print(f"Error: El archivo {filename} no fue encontrado.")
    except Exception as e:
        print(f"Ha ocurrido un error: {e}")

def main():
    if len(sys.argv) != 2:
        print("Usage: python main.py <filename>")
        sys.exit(1)

    filename = sys.argv[1]
    print(f"evaluando el fichero {filename}:")
    read_and_print_file(filename)
    try:
        input_stream = FileStream(filename)
        lexer = MiniSchemeLexer(input_stream)
        token_stream = CommonTokenStream(lexer)
        parser = MiniSchemeParser(token_stream)
        tree = parser.program()

        print("parsed tree:", tree.toStringTree(recog=parser))

        evaluator = Evaluator()
        result = evaluator.evaluate(tree)

        print("Result:", result)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
