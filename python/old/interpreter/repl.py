from antlr4 import InputStream, CommonTokenStream
from grammar.generated.MiniSchemeLexer import MiniSchemeLexer
from grammar.generated.MiniSchemeParser import MiniSchemeParser
from src.evaluator import Evaluator

def repl():
    evaluator = Evaluator()
    print("Mini-Scheme Interpreter. Type 'exit' to quit.")
    while True:
        try:
            line = input(">> ")
            if line.lower() == 'exit':
                break

            input_stream = InputStream(line)
            lexer = MiniSchemeLexer(input_stream)
            token_stream = CommonTokenStream(lexer)
            parser = MiniSchemeParser(token_stream)
            tree = parser.program()

            result = evaluator.evaluate(tree)
            print(result)
        except Exception as e:
            print(f"Error: {e}")


if __name__ == "__main__":
    repl()

