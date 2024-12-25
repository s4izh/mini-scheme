from src.grammar.generated.MiniSchemeParser import MiniSchemeParser
from src.grammar.generated.MiniSchemeListener import MiniSchemeListener
from src.grammar.generated.MiniSchemeLexer import MiniSchemeLexer
from antlr4 import InputStream, CommonTokenStream

class Evaluator:
    def __init__(self):
        self.environment = {}

    def evaluate(self, ctx):
        if isinstance(ctx, MiniSchemeParser.ProgramContext):
            print(f"Es el programa entero: {ctx.getText()}, vamos a mirar cada statement")
            result = None
            for statement in ctx.statement():
                result = self.evaluate(statement)
            return result

        elif isinstance(ctx, MiniSchemeParser.StatementContext):
            print(f"Estamos en un statement, evaluamos su expresion: {ctx.expression().getText()}")
            return self.evaluate(ctx.expression())

        elif isinstance(ctx, MiniSchemeParser.NumberExprContext):
            print(f"Estamos en un número, devolvemos su valor: {ctx.getText()}")
            return int(ctx.getText())

        elif isinstance(ctx, MiniSchemeParser.SymbolExprContext):
            print(f"Estamos en un símbolo, devolvemos su valor: {ctx.getText()}")
            return self.environment.get(ctx.getText(), None)

        elif isinstance(ctx, MiniSchemeParser.OperationExprContext):
            print(f"Estamos en un operador: {ctx.operator().getText()}")
            operator = ctx.operator().getText()
            print(f"Evaluamos recursivamente cada expresion: {[arg.getText() for arg in ctx.expression()]}")
            args = [self.evaluate(arg) for arg in ctx.expression()]
            print(f"Aplicamos el operador: {ctx.operator().getText()} con {args}")
            return self._apply_operator(operator, args)

        elif isinstance(ctx, MiniSchemeParser.DefineExprContext):
            var_name = ctx.SYMBOL().getText()
            var_value = self.evaluate(ctx.expression())
            self.environment[var_name] = var_value
            return var_value

        else:
            raise ValueError(f"Unknown context: {ctx}")

    def _apply_operator(self, operator, args):
        if operator == '+':
            return sum(args)
        elif operator == '-':
            return args[0] - sum(args[1:])
        elif operator == '*':
            result = 1
            for arg in args:
                result *= arg
            return result
        elif operator == '/':
            return args[0] / args[1]
        else:
            raise ValueError(f"Unsupported operator: {operator}")
