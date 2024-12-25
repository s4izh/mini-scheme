# from generated.schemeLexer import schemeLexer
from generated.schemeParser import schemeParser
# from generated.schemeListener import schemeListener
# from antlr4 import InputStream


class Evaluator:
    def __init__(self):
        self.environment = {}

    def evaluate_program(self, ctx):
        """
        Evaluate the top-level program containing multiple expressions.
        """
        result = None
        for expression in ctx.expression():
            result = self.evaluate(expression)
            return result

    def evaluate(self, ctx):
        """
        Recursively evaluates a parse tree node.
        """
        if ctx is None:
            return None

        # Literal evaluation (number, boolean, identifier, or string)
        if isinstance(ctx, schemeParser.LiteralContext):
            return self._evaluate_literal(ctx)

        # Quoted expression
        if isinstance(ctx, schemeParser.QuotedExpressionContext):
            return self._evaluate_quoted(ctx)

        # Generic expression
        if isinstance(ctx, schemeParser.ExpressionContext):
            if ctx.getChildCount() == 1:  # Single child: literal or identifier
                return self.evaluate(ctx.getChild(0))
            elif ctx.getChild(0).getText() == "(":  # S-expression
                return self._evaluate_s_expression(ctx)

        # Special form
        if isinstance(ctx, schemeParser.SpecialFormContext):
            return self._evaluate_special_form(ctx)

        raise ValueError(f"Unhandled context: {type(ctx)}")

    def _evaluate_literal(self, ctx):
        """
        Evaluates literals such as numbers, booleans, or identifiers.
        """
        if ctx.NUMBER():
            return float(ctx.NUMBER().getText()) if '.' in ctx.NUMBER().getText() else int(ctx.NUMBER().getText())
        elif ctx.BOOLEAN():
            return ctx.BOOLEAN().getText() == '#t'
        elif ctx.IDENTIFIER():
            identifier = ctx.IDENTIFIER().getText()
            if identifier in self.environment:
                return self.environment[identifier]
            else:
                raise ValueError(f"Unbound identifier: {identifier}")
        elif ctx.STRING():
            return ctx.STRING().getText().strip('"')
        else:
            raise ValueError(f"Unhandled literal: {ctx.getText()}")

    def _evaluate_quoted(self, ctx):
        """
        Evaluates a quoted expression by returning it as is.
        """
        return ctx.expression().getText()  # Return the quoted expression as a string or symbolic representation

    def _evaluate_s_expression(self, ctx):
        """
        Evaluates an S-expression.
        """
        children = ctx.expression()
        operator = self.evaluate(children[0])  # First child is the operator or function name
        args = [self.evaluate(child) for child in children[1:]]  # Evaluate arguments


        if callable(operator):
            return operator(*args)  # Apply function if operator is callable
        else:
            return self._apply_operator(operator, args)

    def _evaluate_special_form(self, ctx):
        """
        Evaluates special forms like define or if.
        """
        if ctx.getChild(0).getText() == "define":
            return self._evaluate_define(ctx)
        elif ctx.getChild(0).getText() == "if":
            return self._evaluate_if(ctx)
        else:
            raise ValueError(f"Unhandled special form: {ctx.getChild(0).getText()}")

    def _evaluate_define(self, ctx):
        """
        Handles the `define` special form for variables and functions.
        """
        # Check if defining a function or variable
        if ctx.IDENTIFIER():
            # Variable definition: (define x 10)
            identifier = ctx.IDENTIFIER().getText()
            value = self.evaluate(ctx.expression(0))
            self.environment[identifier] = value
            return None
        elif ctx.getChild(1).getText() == "(":
            # Function definition: (define (name params) body)
            func_name = ctx.IDENTIFIER(0).getText()
            params = [param.getText() for param in ctx.parameters().IDENTIFIER()]
            body = ctx.expression(0)

            # Store the function as a lambda
            def func(*args):
                local_env = dict(zip(params, args))
                self.environment.update(local_env)
                return self.evaluate(body)

            self.environment[func_name] = func
            return None
        else:
            raise ValueError("Invalid define syntax")

    def _evaluate_if(self, ctx):
        """
        Handles the `if` special form.
        """
        condition = self.evaluate(ctx.expression(0))
        if condition:
            return self.evaluate(ctx.expression(1))
        elif ctx.expression(2):
            return self.evaluate(ctx.expression(2))
        return None

    def _apply_operator(self, operator, args):
        """
        Applies an operator like +, -, *, / to a list of arguments.
        """
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
            result = args[0]
            for arg in args[1:]:
                result /= arg
            return result
        else:
            raise ValueError(f"Unsupported operator: {operator}")
